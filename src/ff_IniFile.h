#ifndef _INIFILE_H
#define _INIFILE_H

#define INIFILE_VERSION "1.0.1 BM"

#include "ff_sys_config.h"


#ifdef FF_ARDUINO
#include "SD.h"
#include "Ethernet.h"
#endif
#ifdef FF_SIMULATOR
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#endif

class IniFileState;

class IniFile {
public:
  enum error_t {
    errorNoError = 0,
    errorFileNotFound,
    errorFileNotOpen,
    errorBufferTooSmall,
    errorSeekError,
    errorSectionNotFound,
    errorKeyNotFound,
    errorEndOfFile,
    errorUnknownError,
  };

  static const uint8_t maxFilenameLen;

  // Create an IniFile object. It isn't opened until open() is called on it.
#ifdef FF_ARDUINO
  IniFile(const char* filename, uint8_t mode = FILE_READ, bool caseSensitive = false);
  ~IniFile();
#endif
#ifdef FF_SIMULATOR
  IniFile(const char* filename, char* mode, bool caseSensitive = false);
   ~IniFile();
#endif

  inline bool open(void); // Returns true if open succeeded
  inline void close(void);

  inline bool isOpen(void) const;

  inline error_t getError(void) const;
  inline void clearError(void) const;
  // Get the file mode (FILE_READ/FILE_WRITE)
#ifdef FF_ARDUINO
  inline uint8_t getMode(void);
#endif
#ifdef FF_SIMULATOR
  inline char* getMode(void);
#endif
  // Get the filename asscoiated with the ini file object
  inline const char* getFilename(void) const;

  bool validate(char* buffer, size_t len) const;
  
  // Get value from the file, but split into many short tasks. Return
  // value: false means continue, true means stop. Call getError() to
  // find out if any error
  bool getValue(const char* section, const char* key,
		   char* buffer, size_t len, IniFileState &state) const;

  // Get value, as one big task. Return = true means value is present
  // in buffer
  bool getValue(const char* section, const char* key,
		  char* buffer, size_t len) const; 

  // Get the value as a string, storing the result in a new buffer
  // (not the working buffer)
  bool getValue(const char* section, const char* key,
		   char* buffer, size_t len, char *value, size_t vlen) const;

  // Get a boolean value
  bool getValue(const char* section, const char* key,
		   char* buffer, size_t len, bool& b) const;

  // Get an integer value
  bool getValue(const char* section, const char* key,
		   char* buffer, size_t len, int& val) const;

  // Get a uint16_t value
  bool getValue(const char* section, const char* key,
		   char* buffer, size_t len, uint16_t& val) const;

  // Get a long value
  bool getValue(const char* section, const char* key,
		   char* buffer, size_t len, long& val) const;

  bool getValue(const char* section, const char* key,
		   char* buffer, size_t len, unsigned long& val) const;

  // Get a float value
  bool getValue(const char* section, const char* key,
		   char* buffer, size_t len, float& val) const;
  
  bool getIPAddress(const char* section, const char* key,
		       char* buffer, size_t len, uint8_t* ip) const;
  
#if defined(ARDUINO) && ARDUINO >= 100
  bool getIPAddress(const char* section, const char* key,
		       char* buffer, size_t len, IPAddress& ip) const;
#endif

  bool getMACAddress(const char* section, const char* key,
			char* buffer, size_t len, uint8_t mac[6]) const;

  // Utility function to read a line from a file, make available to all
  //static int8_t readLine(File &file, char *buffer, size_t len, uint32_t &pos);
#ifdef FF_ARDUINO
  static error_t readLine(File &file, char *buffer, size_t len, uint32_t &pos);
#endif
#ifdef FF_SIMULATOR
  static error_t readLine(FILE* file, char *buffer, size_t len, uint32_t &pos);
#endif
  static bool isCommentChar(char c);
  static char* skipWhiteSpace(char* str);
  static void removeTrailingWhiteSpace(char* str);

  bool getCaseSensitive(void) const;
  void setCaseSensitive(bool cs);
  
  protected:
  // True means stop looking, false means not yet found
  bool findSection(const char* section, char* buffer, size_t len,	
		      IniFileState &state) const;
  bool findKey(const char* section, const char* key, char* buffer,
		 size_t len, char** keyptr, IniFileState &state) const;


private:
  char _filename[INI_FILE_MAX_FILENAME_LEN];
#ifdef FF_ARDUINO
  uint8_t _mode;
#endif
#ifdef FF_SIMULATOR
  char _mode[3];
#endif
  mutable error_t _error;
#ifdef FF_ARDUINO
  mutable File _file;
#endif
#ifdef FF_SIMULATOR
  mutable FILE* _file;
#endif
  bool _caseSensitive;
};

bool IniFile::open(void)
{
  if (_file)
#ifdef FF_ARDUINO
	  _file.close();
#endif
#ifdef FF_SIMULATOR
  	  fclose(_file);  //used to be a problem here - now seems fixed
#endif
#ifdef FF_ARDUINO
  _file = SD.open(_filename, _mode);
#endif
#ifdef FF_SIMULATOR
  _file = fopen(_filename, _mode);
#endif
  if (isOpen()) {
    _error = errorNoError;
    return true;
  }
  else {
    _error = errorFileNotFound;
    return false;
  }
}

void IniFile::close(void) {
#ifdef FF_ARDUINO
	if (_file) {
		_file.close();
		SD.end();
	}
#endif
#ifdef FF_SIMULATOR
	if (_file) {
		fclose(_file);
		_file = NULL;
	}
#endif

}

bool IniFile::isOpen(void) const
{
	return _file;
}

IniFile::error_t IniFile::getError(void) const
{
  return _error;
}

void IniFile::clearError(void) const
{
  _error = errorNoError;
}

#ifdef FF_ARDUINO
uint8_t IniFile::getMode(void)
{
  return _mode;
}
#endif

#ifdef FF_SIMULATOR
char* IniFile::getMode(void)
{
  return _mode;
}
#endif

const char* IniFile::getFilename(void) const
{
  return _filename;
}



class IniFileState {
public:
  IniFileState();

private:
  enum {funcUnset = 0,
	funcFindSection,
	funcFindKey,
  };
	
  uint32_t readLinePosition;
  uint8_t getValueState;

  friend class IniFile;
};

  
#endif

