#include <ferite.h>
#include "serial_header.h"

#define NUMBAUDCONSTS 19

/* On some systems, the baud constants equal the actual baud rate (eg. 
 * B9600 == 9600) but that is by no means guaranteed, so we must convert
 * between the constants and the actual baud rates. */

speed_t consts[NUMBAUDCONSTS] = { B0, B50, B75, B110, B134, B150, B200, B300,
				B600, B1200, B1800, B2400, B4800, B9600,
				B19200, B38400, B57600, B115200, B230400 };

speed_t bauds[NUMBAUDCONSTS] = { 0, 50, 75, 110, 134, 150, 200, 300, 600, 1200,
				1800, 2400, 4800, 9600, 19200, 38400, 57600,
				115200, 230400 };

speed_t consttobaud(speed_t constant)
{
	int i;

	for(i = 0; i < NUMBAUDCONSTS; i++)
		if(consts[i] == constant) return bauds[i];

	return -1;
}

speed_t baudtoconst(speed_t baud)
{
	int i;

	for(i = 0; i < NUMBAUDCONSTS; i++)
		if(bauds[i] == baud) return consts[i];

	return -1;
}
