/*
 * sockets.c
 *
 *  Created on: Jan 23, 2016
 *      Author: joe
 */


#include "sockets.h"

int sendQuery(unsigned char *sendBuff, int len, unsigned char **recvBuff)
{
	int s;
	int flag = 0;

	char *dstIp = "127.0.0.1"; // cat resolv.conf or similiar
	unsigned short dstPort = 53;
	struct sockaddr_in dstAddr;

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket");
		return 0;
	}

	// destination info
	dstAddr.sin_family = AF_INET;
	dstAddr.sin_port = htons(dstPort);
	dstAddr.sin_addr.s_addr = inet_addr(dstIp);

	// send the udp datagram
	if (sendto(s, sendBuff, len, 0, (struct sockaddr *)&dstAddr, sizeof(dstAddr)) < 0)
	{
		perror("sendto failed");
		return 0;
	}

	// close the socket descriptor
	close(s);

	// free malloc'd socketBuffer
	// if we free here, then the other functions which rely on global
	// buffer can't use it
	//free(sockBuff);
	//{
	//	printf("	 failed for free(sockBuff) \n");
	//}
	// close socket to do or not to do ?
	// when sleep = 0 .. failed to create dgram socket: Too many open files
	// closing causes a new source port to be generated per request?
	///close(s); // will close and then once reopened get new src port every tx?

	return flag;
}
