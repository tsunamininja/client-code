// global vars

// for function definitions and include files relevant to functions
// called in this file

#include <stdio.h>			// printf
#include "bufferHelper.h"

/*
 * put<DataType> .. more like append<DataType>
 */

// pretty much same as memcpy but also is aware of current position of buff
unsigned int putChar(unsigned char *buff,
						unsigned char *val,
							unsigned int *pos)
{
	// store the value specified by val into the buffer
	// specified by buff starting at the position
	// specified by pos
	unsigned int ret = 0;
	unsigned int position = *pos;
	unsigned int length = sizeof(unsigned char);

	unsigned char *blah = memcpy(&buff[position], val, length);

	// get return value
	if (blah == NULL)
		ret = 0;

	else
		ret = 1;

	// increment position index counter
	position += length;

	// assign the current value of position to the contents
	// pointed to by pos
	*pos = position;

	return ret;
}


// pretty much same as memcpy but also is aware of current position of buff
unsigned int putShort(unsigned char *buff,
						unsigned short *val,
							unsigned int *pos)
{
	// store the value specified by val into the buffer
	// specified by buff starting at the position
	// specified by pos
	unsigned int ret = 0;
	unsigned int position = *pos;
	unsigned int length = sizeof(unsigned short);

	unsigned char *blah = memcpy(&buff[position], val, length);

	// get return value
	if (blah == NULL)
		ret = 0;

	else
		ret = 1;

	// increment position index counter
	position += length;


	// assign the current value of position to the contents
	// pointed to by pos
	*pos = position;

	return ret;
}


// pretty much same as memcpy but also is aware of current position of buff
unsigned int putString(unsigned char *destBuff,
							unsigned char *srcBuff,
								unsigned int count,
										unsigned int *pos)
{
	// store the source string specified by srcBuff into the destination buffer
	// specified by destBuff starting at the position specified by pos
	unsigned int position = *pos;
	unsigned char *dummyPtr = memcpy(&destBuff[position], srcBuff, count);
	unsigned int ret = 0;

	// get return value
	if (dummyPtr == NULL)
		ret = 0;

	else
		ret = 1;

	// increment position index counter by how many bytes we "appended"
	position += count;

	// assign the current value of position to the contents
	// pointed to by pos
	*pos = position;

	return ret;
}

unsigned int copyString(unsigned char *destBuff,
							unsigned char *srcBuff,
								unsigned int count)
{
	// store the source string specified by srcBuff into the destination buffer
	// specified by destBuff starting at the position specified by pos
	unsigned char *dummyPtr = memcpy(destBuff, srcBuff, count);
	unsigned int ret = 0;

	// get return value
	if (dummyPtr == NULL)
	{
		ret = 0;
	}
	else // success
		ret = 1;

	return ret;
}

unsigned int seekToChar(unsigned char *qNameArg,
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

	while(qNameArg[seekBytes] != seekValueArg)
	{
		printf("qNameArg[%u] char-> %c \n", seekBytes, qNameArg[seekBytes]);
		seekBytes++;
	}

	return seekBytes;
}
