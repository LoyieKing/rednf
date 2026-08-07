#ifndef UDP_H_
#define UDP_H_

#include <errno.h>

typedef int Socket;

const Socket INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;
const int udpMaxMessageLength = 16384;

inline int getErrno()
{
    return errno;
}

Socket openPort(unsigned short port, unsigned int interfaceIp);
bool getMessage(Socket fd, char* buf, int* len, unsigned int* srcIp, unsigned short* srcPort);
bool sendMessage(Socket fd, char* buf, int l, unsigned int dstIp, unsigned short dstPort);
void initNetwork();

#endif // UDP_H_
