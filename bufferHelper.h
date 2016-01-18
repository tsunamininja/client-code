/*
 * bufferHelper.h
 *
 *  Created on: Jan 15, 2016
 *      Author: joe
 */

#ifndef BUFFERHELPER_H_
#define BUFFERHELPER_H_


// include statements
#include <string.h> // memcpy

// global variables

// function definitions
unsigned int putChar(unsigned char* buff,
						unsigned char *val,
							unsigned int *pos);

unsigned int putShort(unsigned char* buff,
						unsigned short *val,
							unsigned int *pos);

unsigned int putString(unsigned char *destBuff,
							unsigned char *srcBuff,
								unsigned int count,
									unsigned int *pos);

unsigned int copyString(unsigned char *destBuff,
							unsigned char *srcBuff,
								unsigned int count);

unsigned int seekToChar(unsigned char *qNameArg,
								unsigned char seekValue);












#endif /* BUFFERHELPER_H_ */
