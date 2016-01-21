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
							unsigned int *curPos);


unsigned int putShort(unsigned char* buff,
						unsigned short *val,
							unsigned int *curPos);


unsigned int putString(unsigned char *destBuff,
							unsigned int destBuffSize,
								unsigned char *srcBuff,
									unsigned int count,
										unsigned int *curPos);


unsigned int copyString(unsigned char *destBuff,
							unsigned int destBuffSize,
								unsigned char *srcBuff,
									unsigned int count);


unsigned int seekToChar(unsigned char *qNameArg,
								unsigned char seekValue);


void stringPrinter(unsigned char *buff, unsigned int len);

/* byte buffer related operations */

unsigned int getPosition();

void resetPosition();










#endif /* BUFFERHELPER_H_ */
