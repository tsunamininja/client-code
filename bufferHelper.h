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
int putChar(unsigned char* buff,
				unsigned char *val,
					unsigned int *curPos);

int fetchChar(unsigned char *val,
					unsigned char *buff,
						unsigned int *curPos);

int putShort(unsigned char* buff,
				unsigned short *val,
					unsigned int *curPos);

int fetchShort(unsigned short *val,
					unsigned char *buff,
						unsigned int *curPos);

int fetchInt(unsigned int *val,
				unsigned char *buff,
					unsigned int *curPos);


int putString(unsigned char *destBuff,
				unsigned char *srcBuff,
					unsigned int count,
						unsigned int *curPos);

unsigned char *appendString(unsigned char *_str1,
								unsigned char *_str2);

int fetchString(unsigned char *destBuff,
					unsigned char *srcBuff,
							unsigned int count,
									unsigned int *curPos);


int copyString(unsigned char *destBuff,
					unsigned char *srcBuff,
						unsigned int count);


int seekToChar(unsigned char *qNameArg,
					unsigned char seekValue);


void stringPrinter(unsigned char *buff, unsigned int len);

/* byte buffer related operations */


unsigned int getPosition();


void resetPosition();


#endif /* BUFFERHELPER_H_ */
