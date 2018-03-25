#include "Arduino.h"
#include "out.h"
#include "itch.h"

ITCH itch;

#ifdef ARDUINO
#ifdef DEBUG

void M(char strn[]) {
	char str[MAX_OUTPUT_LINE_SIZE];
/* This function places the current value of the heap and stack pointers in the
 * variables. You can call it from any place in your code and save the data for
 * outputting or displaying later. This allows you to check at different parts of
 * your program flow.
 * The stack pointer starts at the top of RAM and grows downwards. The heap pointer
 * starts just above the static variables etc. and grows upwards. SP should always
 * be larger than HP or you'll be in big trouble! The smaller the gap, the more
 * careful you need to be. Julian Gall 6-Feb-2009.
 */

	uint8_t * heapptr, * stackptr;

	//bll_tail = (uint8_t *)GetLastBlockAddr();
	stackptr = (uint8_t *)malloc(4);          // use stackptr temporarily
	heapptr = stackptr;                     // save value of heap pointer
	free(stackptr);      // free up the memory again (sets stackptr to 0)
	stackptr =  (uint8_t *)(SP);           // save value of stack pointer
	sprintf(str, "SP: %d >< %d :HP \t\t", (int)stackptr, (int)heapptr);
	strcat(str, strn);
	Serial.write(str);
}
#endif
#endif


//void ITCHWriteLine(char *str) {
//	char cb_str[MAX_OUTPUT_LINE_SIZE];
//	strcpy(cb_str, str);
//	itch.WriteLine(cb_str);
//}

void setup()
{
	 Serial.begin(9600);

	 itch.Begin(ITCH_INTERACTIVE);
}

void loop()
{
	itch.Poll();
}
