/*
 * sockets.c
 *
 *  Created on: Jan 23, 2016
 *      Author: joe
 */


#include "sockets.h"

/*
 * function name - sendQuery
 *
 * arguments - _sendBuff => pointer to buffer containing data to send
 * 			   _sendBuffLen => about of bytes to send
 * 			   _recvBuff => pointer to pointer of char* for caller to fetch reply
 *
 * return value - returns the amount of bytes received.. if 0 -- cant parse reply..
 *
 * comments - Incase the destination dns server shutsdown, we may receive
 *            a SIGPIPE signal, if we add the flag MSG_NOSIGNAL, perhaps
 *            sendto will just return silently.. ?
 *
 *            Make sure to malloc *_recvBuff because caller passes in null val
 *
 *            recvfrom will block until it receives a reply from sendto
 *
 * bugs - Do not use a macro expansion for a number when arg is expect size_t
 * !BUG - if peer server is down, set a timeout before not blocking??
 */
unsigned char *sendQuery(unsigned char *_sendBuff,
							int _sendBuffLen,
								int *bytesRecv)
{
	int s = 0;
	int flag = 0;
	int recvBuffSize = RECV_SIZE;
	unsigned char *recvBuff = NULL;

	char *dstIp = "127.0.0.1"; // cat resolv.conf or similiar
	unsigned short dstPort = 53;
	struct sockaddr_in dstAddr;

	// allocate buffer for receiving.. free in caller !
	recvBuff = malloc(sizeof(unsigned char) * RECV_SIZE);

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket");
		return 0;
	}

	// destination info
	dstAddr.sin_family = AF_INET;
	dstAddr.sin_port = htons(dstPort);
	dstAddr.sin_addr.s_addr = inet_addr(dstIp);

	// send the udp datagram
	if(sendto(s, _sendBuff, _sendBuffLen, 0,
					(struct sockaddr *)&dstAddr, sizeof(dstAddr)) < 0)
	{
		perror("sendto failed");
		return 0;
	}

	// clear _recvBuff from last message
	if(recvBuff != NULL)
		memset(recvBuff, '\0', recvBuffSize);

	// contents of _recvBuff is a pointer containing NULL,
	// _recvBuff = address of stack variable
	// *_recvBuff = contents of stack variable which is address of main::buffer
	// with the address of mains buffer variable, we can modify the contents of it

	// if the caller wants a variable modifeid, it needs to pass address of var
	// if it just passes variable.. the contents of variable "null" are passed
	// can't change the contents of nothing.

	// send the udp datagram response
	// MSG_DONTWAIT incase server is down or unavailable.. don't block for ever.
	if( (*bytesRecv = recvfrom(s, recvBuff, RECV_SIZE, 0, 0, 0)) < 0)
	{
		perror("recvfrom failed");
		return 0;
	}

	// close the socket descriptor
	close(s);

	return recvBuff;
}
