/*
 * sockets.h
 *
 *  Created on: Jan 23, 2016
 *      Author: joe
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <sys/socket.h>		// socket
#include <string.h>			// memset
#include <unistd.h>			// close
#include <stdlib.h>			// malloc
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

// defines
#define RECV_SIZE 512

int sendQuery(unsigned char *sendBuff, int len, unsigned char **recvBuff);



#endif /* SOCKETS_H_ */


