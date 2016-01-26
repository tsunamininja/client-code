// global vars

// for function definitions and include files relevant to functions
// called in this file

#include <stdio.h>			// printf
#include <stdlib.h>			// exit
#include "bufferHelper.h"

/*
 * put<DataType> .. more like append<DataType>
 *
 * MAINLY USED TO APPEND DATA TYPES ONTO AN EXISTING CHAR BUFFER
 */

// pretty much same as memcpy but also is aware of current position of buff
int putChar(unsigned char *buff,
				unsigned char *val,
						unsigned int *curPos)
{
	// store the value specified by val into the buffer
	// specified by buff starting at the position
	// specified by pos
	unsigned int ret = 0;
	unsigned int length = sizeof(unsigned char);

	printf("@[putChar] -- about to store char: %c \n", *val);

	unsigned char *blah = memcpy(&buff[*curPos], val, length);

	// get return value
	if (blah == NULL)
		ret = 0;

	else
		ret = 1;

	// increment position index counter
	*curPos += length;

	return ret;
}

// buff = buffer containing data to fetch
// val
int fetchChar(unsigned char *val,
				unsigned char *buff,
					unsigned int *curPos)
{
	// store the value specified by val into the buffer
	// specified by buff starting at the position
	// specified by pos
	unsigned int ret = 0;
	unsigned int length = sizeof(unsigned char);

	unsigned char *blah = memcpy(val, &buff[*curPos], length);

	//printf("@[putChar] -- about to return char: %c \n", fetchedChar);

	// get return value


	// increment position index counter
	*curPos += length;

	return ret;
}


// pretty much same as memcpy but also is aware of current position of buff
int putShort(unsigned char *buff,
					unsigned short *val,
							unsigned int *curPos)
{
	// store the value specified by val into the buffer
	// specified by buff starting at the position
	// specified by pos
	unsigned int ret = 0;
	unsigned int length = sizeof(unsigned short);
	unsigned int position = 0;

	unsigned char *blah = memcpy(&buff[*curPos], val, length);

	// get return value
	if (blah == NULL)
		ret = 0;

	else
		ret = 1;

	// increment position index counter
	*curPos += length;

	return ret;
}

int fetchShort(unsigned short *val,
					unsigned char *buff,
						unsigned int *curPos)
{
	// store the value specified by val into the buffer
	// specified by buff starting at the position
	// specified by pos
	unsigned int ret = 0;
	unsigned int length = sizeof(unsigned short);

	unsigned char *blah = memcpy(val, &buff[*curPos], length);

	//printf("@[putChar] -- about to return char: %c \n", fetchedChar);

	// get return value


	// increment position index counter
	*curPos += length;

	return ret;
}

int fetchInt(unsigned int *val,
				unsigned char *buff,
					unsigned int *curPos)
{
	// store the value specified by val into the buffer
	// specified by buff starting at the position
	// specified by pos
	unsigned int ret = 0;
	unsigned int length = sizeof(unsigned int);

	unsigned char *blah = memcpy(val, &buff[*curPos], length);

	// get return value

	// increment position index counter
	*curPos += length;

	return ret;
}

// pretty much same as memcpy but also is aware of current position of buff
// used for parsing and appending onto things
int putString(unsigned char *destBuff,
					unsigned char *srcBuff,
							unsigned int count,
									unsigned int *curPos)
{
	// store the source string specified by srcBuff into the destination buffer
	// specified by destBuff starting at the position specified by pos
	unsigned char *dummyPtr = memcpy(&destBuff[*curPos], srcBuff, count);
	unsigned int ret = 0;

	// get return value
	if (dummyPtr == NULL)
		ret = -1;

	else
		ret = 0;

	// increment position index counter by how many bytes we "appended"
	*curPos += count;

	return ret;
}

// grab a string from srcBuff -- stores it into DestBuff
// "getString"
int fetchString(unsigned char *destBuff,
					unsigned char *srcBuff,
							unsigned int count,
									unsigned int *curPos)
{
	// store the source string specified by srcBuff into the destination buffer
	// specified by destBuff starting at the position specified by pos
	unsigned char *dummyPtr = memcpy(destBuff, &(srcBuff[*curPos]), count);
	unsigned int ret = 0;

	// get return value
	if (dummyPtr == NULL)
		ret = -1;

	else
		ret = 0;

	// increment position index counter by how many bytes we "appended"
	*curPos += count;

	return ret;
}


// should be copy bytes because sometimes we are copying non-string values
// where strcpy would fail
int copyString(unsigned char *destBuff,
					unsigned char *srcBuff,
						unsigned int count)
{
	// store the source string specified by srcBuff into the destination buffer
	// specified by destBuff starting at the position specified by pos
	// **BUG ** if count > deskBuff.capacity
	unsigned char *dummyPtr = memcpy(destBuff, srcBuff, count);
	unsigned int ret = 0;

	// get return value
	if (dummyPtr == NULL)
		ret = -1;

	else // success
		ret = 0;

	return ret;
}

int seekToChar(unsigned char *qNameArg,
					unsigned char seekValueArg)
{
	printf("==============DEBUG=============== \n");
	printf("function name - seekToChar    	   \n");
	printf("qNameArg -> %s		\n", qNameArg);
	printf("seekValueArg -> %c 	\n", seekValueArg);
	printf("================================== \n");

	// from qNameArg start.. how many bytes does it take
	// until we reach the seekValueArg
	unsigned int seekBytes = 0;
	unsigned int c;



	return seekBytes;
}

/*
 * function name - stringPrinter
 *
 * function purpose - print out the hex and ascii representation of each byte
 * in a string
 *
 * arg1 - pointer to string
 * arg2 - length of string
 *
 * return value - void
 *
 * comments - tbd
 *
 * enhancements - tbd
 */
void stringPrinter(unsigned char *buff, unsigned int len)
{
	//printf("[@] stringPrinter \n");
	//printf("size of char array: %d bytes \n\n", sizeof(in_string));

	for (int i=0; i<len; i++) // actually prints length
	{
	    //printf("index> %u char> %c  hex> %x \n", i, buff[i], buff[i]);
		printf("%x_", buff[i]);
	}
}
