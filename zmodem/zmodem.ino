
#include "zmodem.h"


#include "Arduino.h"
#include <avr/pgmspace.h>

//#include <SPI.h>

// Arghhh. These three links have disappeared!
// See this page for the original code:
// http://www.raspberryginger.com/jbailey/minix/html/dir_acf1a49c3b8ff2cb9205e4a19757c0d6.html
// From: http://www.raspberryginger.com/jbailey/minix/html/zm_8c-source.html
// docs at: http://www.raspberryginger.com/jbailey/minix/html/zm_8c.html
// The minix files here might be the same thing:
// http://www.cise.ufl.edu/~cop4600/cgi-bin/lxr/http/source.cgi/commands/zmodem/

#include <SPI.h>

#if ENABLE_SOFTWARE_SPI_CLASS  // Must be set in SdFat/SdFatConfig.h
// Pin numbers in templates must be constants.
const uint8_t SOFT_MISO_PIN = 12;
const uint8_t SOFT_MOSI_PIN = 11;
const uint8_t SOFT_SCK_PIN  = 13;
// Chip select may be constant or RAM variable.
const uint8_t SD_CHIP_SELECT_PIN = 10;
// SdFat software SPI template
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;
#endif



//#include <SdFatUtil.h>
#include "SdFat.h"
#include "zmodem_config.h"
#include "zmodem_zm.h"
#include "zmodem_fixes.h"


//SdFatSoftSpi sd;

#ifdef SFMP3_SHIELD
#include <SFEMP3Shield.h>
#include <SFEMP3ShieldConfig.h>
#include <SFEMP3Shieldmainpage.h>

SFEMP3Shield mp3;
#endif

#define error(s) sd.errorHalt(s)

extern int Filesleft;
extern long Totalleft;

extern SdFile fout;

// Dylan (monte_carlo_ecm, bitflipper, etc.) - This function was added because I found
// that SERIAL_TX_BUFFER_SIZE was getting overrun at higher baud rates.  This modified
// Serial.print() function ensures we are not overrunning the buffer by flushing if
// it gets more than half full.

size_t DSERIALprint(const __FlashStringHelper *ifsh)
{
  PGM_P p = reinterpret_cast<PGM_P>(ifsh);
  size_t n = 0;
  while (1) {
    unsigned char c = pgm_read_byte(p++);
    if (c == 0) break;
    if (DSERIAL.availableForWrite() > SERIAL_TX_BUFFER_SIZE / 2) DSERIAL.flush();
    if (DSERIAL.write(c)) n++;
    else break;
  }
  return n;
}

#define DSERIALprintln(_p) ({ DSERIALprint(_p); DSERIAL.write("\r\n"); })

void help(void)
{
  DSERIALprint(Progname);
  DSERIALprint(F(" - Transfer rate: "));
  DSERIAL.flush(); DSERIAL.println(ZMODEM_SPEED); DSERIAL.flush();
  DSERIALprintln(F("Available Commands:")); DSERIAL.flush();
  DSERIALprintln(F("HELP     - Print this list of commands")); DSERIAL.flush();
  DSERIALprintln(F("DIR      - List files in current working directory - alternate LS")); DSERIAL.flush();
  DSERIALprintln(F("PWD      - Print current working directory")); DSERIAL.flush();
  DSERIALprintln(F("CD       - Change current working directory")); DSERIAL.flush();
#ifdef ARDUINO_SMALL_MEMORY_INCLUDE_FILE_MGR
  DSERIALprintln(F("DEL file - Delete file - alternate RM")); DSERIAL.flush();
  DSERIALprintln(F("MD  dir  - Create dir - alternate MKDIR")); DSERIAL.flush();
  DSERIALprintln(F("RD  dir  - Delete dir - alternate RMDIR")); DSERIAL.flush();
#endif
#ifdef ARDUINO_SMALL_MEMORY_INCLUDE_SZ
  DSERIALprintln(F("SZ  file - Send file from Arduino to terminal (* = all files)")); DSERIAL.flush();
#endif
#ifdef ARDUINO_SMALL_MEMORY_INCLUDE_RZ  
  DSERIALprintln(F("RZ       - Receive a file from terminal to Arduino (Hyperterminal sends this")); DSERIAL.flush();
  DSERIALprintln(F("              automatically when you select Transfer->Send File...)")); DSERIAL.flush();
#endif
}

SdFile fout;
//dir_t *dir ;

// Dylan (monte_carlo_ecm, bitflipper, etc.) - The way I made this sketch in any way operate on
// a board with only 2K of RAM is to borrow the SZ/RZ buffer for the buffers needed by the main
// loop(), in particular the file name parameter and the SdFat directory entry.  This is very
// unorthodox, but now it works on an Uno.  Please see notes in zmodem_config.h for limitations

#define name (&oneKbuf[512])
#define dir ((dir_t *)&oneKbuf[256])


void setup()
{

	//pinMode(10,  OUTPUT);
	//digitalWrite(10, HIGH);
	//pinMode(53, OUTPUT);
	//digitalWrite(53, HIGH);
	//pinMode(52, OUTPUT);
	//digitalWrite(52, HIGH);


// NOTE: The following line needs to be uncommented if DSERIAL and ZSERIAL are decoupled again for debugging
  //DSERIAL.begin(9600);

  ZSERIAL.begin(ZMODEM_SPEED);
  ZSERIAL.setTimeout(TYPICAL_SERIAL_TIMEOUT);

  //DSERIALprintln(Progname);
  
  //DSERIALprint(F("Transfer rate: "));
  //DSERIALprintln(ZMODEM_SPEED);

  //Initialize the SdCard.
//DSERIALprintln(F("About to initialize SdCard"));
  if(!sd.begin(SD_SEL, SPI_FULL_SPEED)) sd.initErrorHalt(&DSERIAL);
  // depending upon your SdCard environment, SPI_HALF_SPEED may work better.
//DSERIALprintln(F("About to change directory"));
  if(!sd.chdir("/", true)) sd.errorHalt(F("sd.chdir"));
//DSERIALprintln(F("SdCard setup complete"));

  #ifdef SFMP3_SHIELD
  mp3.begin();
  #endif

  sd.vwd()->rewind();

  help();
 
}

int count_files(int *file_count, long *byte_count)
{
  *file_count = 0;
  *byte_count = 0;
  
  sd.vwd()->rewind();

  while (sd.vwd()->readDir(dir) == sizeof(*dir)) {
    // read next directory entry in current working directory

    // format file name
    SdFile::dirName(dir, name);

    // remember position in directory
    uint32_t pos = sd.vwd()->curPosition();
     
    // open file
    if (!fout.open(name, O_READ)) error(F("file.open failed"));
    
    // restore root position
    else if (!sd.vwd()->seekSet(pos)) error(F("seekSet failed"));
  
    else if (!fout.isDir()) {
      *file_count = *file_count + 1;
      *byte_count = *byte_count + fout.fileSize();
    }
     
    fout.close();
  }
  return 0;
}

void loop(void)
{
  char *cmd = oneKbuf;
  char *param;

  *cmd = 0;
  while (DSERIAL.available()) DSERIAL.read();
  
  char c = 0;
  while(1) {
    if (DSERIAL.available() > 0) {
      c = DSERIAL.read();
      if ((c == 8 or c == 127) && strlen(cmd) > 0) cmd[strlen(cmd)-1] = 0;
      if (c == '\n' || c == '\r') break;
      DSERIAL.write(c);
      if (c != 8 && c != 127) strncat(cmd, &c, 1);
    } else {
      // Dylan (monte_carlo_ecm, bitflipper, etc.) -
      // This delay is required because I found that if I hard loop with DSERIAL.available,
      // in certain circumstances the Arduino never sees a new character.  Various forum posts
      // seem to confirm that a short delay is required when using this style of reading
      // from Serial
      delay(20);
    }
  }
   
  param = strchr(cmd, 32);
  if (param > 0) {
    *param = 0;
    param = param + 1;
  } else {
    param = &cmd[strlen(cmd)];
  }

  strupr(cmd);
  DSERIAL.println();
//  DSERIALprintln(command);
//  DSERIALprintln(parameter);

  if (!strcmp_P(cmd, PSTR("HELP"))) {
    
    help();
    
  } else if (!strcmp_P(cmd, PSTR("DIR")) || !strcmp_P(cmd, PSTR("LS"))) {
    DSERIALprintln(F("Directory Listing:"));

    sd.vwd()->rewind();

    while (sd.vwd()->readDir(dir) == sizeof(*dir)) {
      // read next directory entry in current working directory
  
      // format file name
      SdFile::dirName(dir, name);

      DSERIAL.flush(); DSERIAL.print(name); DSERIAL.flush();
      for (uint8_t i = 0; i < 16 - strlen(name); ++i) DSERIALprint(F(" "));
      if (!(dir->attributes & DIR_ATT_DIRECTORY)) {
        ultoa(dir->fileSize, name, 10);
        DSERIAL.flush(); DSERIAL.println(name); DSERIAL.flush();
      } else {
        DSERIALprintln(F("DIR"));
      }
      DSERIAL.flush();
    }
    DSERIALprintln(F("End of Directory"));
 
  } else if (!strcmp_P(cmd, PSTR("PWD"))) {
    sd.vwd()->getName(name, 13);
    DSERIALprint(F("Current working directory is "));
    DSERIAL.flush(); DSERIAL.println(name); DSERIAL.flush();
  
  } else if (!strcmp_P(cmd, PSTR("CD"))) {
    if(!sd.chdir(param, true)) {
      DSERIALprint(F("Directory "));
      DSERIAL.flush(); DSERIAL.print(param); DSERIAL.flush();
      DSERIALprintln(F(" not found"));
    } else {
      DSERIALprint(F("Current directory changed to "));
      DSERIAL.flush(); DSERIAL.println(param); DSERIAL.flush();
    }
#ifdef ARDUINO_SMALL_MEMORY_INCLUDE_FILE_MGR
  } else if (!strcmp_P(cmd, PSTR("DEL")) || !strcmp_P(cmd, PSTR("RM"))) {
    if (!sd.remove(param)) {
      DSERIALprint(F("Failed to delete file "));
      DSERIAL.flush(); DSERIAL.println(param); DSERIAL.flush();
    } else {
      DSERIALprint(F("File "));
      DSERIAL.flush(); DSERIAL.print(param); DSERIAL.flush();
      DSERIALprintln(F(" deleted"));
    }
  } else if (!strcmp_P(cmd, PSTR("MD")) || !strcmp_P(cmd, PSTR("MKDIR"))) {
    if (!sd.mkdir(param, true)) {
      DSERIALprint(F("Failed to create directory "));
      DSERIAL.flush(); DSERIAL.println(param); DSERIAL.flush();
    } else {
      DSERIALprint(F("Directory "));
      DSERIAL.flush(); DSERIAL.print(param); DSERIAL.flush();
      DSERIALprintln(F(" created"));
    }
  } else if (!strcmp_P(cmd, PSTR("RD")) || !strcmp_P(cmd, PSTR("RMDIR"))) {
    if (!sd.rmdir(param)) {
      DSERIALprint(F("Failed to remove directory "));
      DSERIAL.flush(); DSERIAL.println(param); DSERIAL.flush();
    } else {
      DSERIALprint(F("Directory "));
      DSERIAL.flush(); DSERIAL.print(param); DSERIAL.flush();
      DSERIALprintln(F(" removed"));
    }
#endif
#ifdef ARDUINO_SMALL_MEMORY_INCLUDE_SZ
  } else if (!strcmp_P(cmd, PSTR("SZ"))) {
//    Filcnt = 0;
    if (!strcmp_P(param, PSTR("*"))) {
      count_files(&Filesleft, &Totalleft);
      sd.vwd()->rewind();

      if (Filesleft > 0) {
        ZSERIAL.print(F("rz\r"));
        sendzrqinit();
        delay(200);
        
        while (sd.vwd()->readDir(dir) == sizeof(*dir)) {
          // read next directory entry in current working directory
      
          // format file name
          SdFile::dirName(dir, name);
                     
          // open file
          if (!fout.open(name, O_READ)) error(F("file.open failed"));
        
          else if (!fout.isDir()) {
            if (wcs(name) == ERROR) {
              delay(500);
              fout.close();
              break;
            }
            else delay(500);
          }
           
          fout.close();
        }
        saybibi();
      } else {
        DSERIALprintln(F("No files found to send"));
      }
    } else if (!fout.open(param, O_READ)) {
      DSERIALprintln(F("file.open failed"));
    } else {
      // Start the ZMODEM transfer
      Filesleft = 1;
      Totalleft = fout.fileSize();
      ZSERIAL.print(F("rz\r"));
      sendzrqinit();
      delay(200);
      wcs(param);
      saybibi();
      fout.close();
    }
#endif
#ifdef ARDUINO_SMALL_MEMORY_INCLUDE_RZ
  } else if (!strcmp_P(cmd, PSTR("RZ"))) {
//    DSERIALprintln(F("Receiving file..."));
    if (wcreceive(0, 0)) {
      DSERIALprintln(F("zmodem transfer failed"));
    } else {
      DSERIALprintln(F("zmodem transfer successful"));
    }
    fout.flush();
    fout.sync();
    fout.close();
#endif
  }
}



