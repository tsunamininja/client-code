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
 * return value - status flag?
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
int sendQuery(unsigned char *_sendBuff,
				int _sendBuffLen,
					unsigned char **_recvBuff)
{
	int s;
	int flag = 0;
	int recvBuffSize = RECV_SIZE;

	char *dstIp = "127.0.0.1"; // cat resolv.conf or similiar
	unsigned short dstPort = 53;
	struct sockaddr_in dstAddr;

	if(_recvBuff != NULL)
		*_recvBuff = malloc(sizeof(unsigned char) * RECV_SIZE);

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
	if(_recvBuff != NULL)
		memset(*_recvBuff, '\0', recvBuffSize);

	// send the udp datagram response
	// MSG_DONTWAIT incase server is down or unavailable.. don't block for ever.
	//if(recvfrom(s, *_recvBuff, RECV_SIZE, 0, 0, 0) < 0)
	//{
	//	perror("recvfrom failed");
	//	return 0;
	//}
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
