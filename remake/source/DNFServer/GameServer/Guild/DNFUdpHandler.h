#ifndef DNFUDPHANDLER_H_
#define DNFUDPHANDLER_H_

#include <sys/socket.h>

#include "PacketHeader.h"

class CUdpHandler;

// from GuildUdp.h
class CUdpHandler
{
public:
    CUdpHandler();
    ~CUdpHandler();
    int InitServerSocket(int port);
    int InitClientSocket();
    bool RecvFromClient(char* buf, int* len, unsigned int* ip, unsigned short* port) const;
    int RecvFromServer(char* buf, int* len, unsigned int* ip, unsigned short* port) const;
    int SendToClient(char* buf, int len, unsigned short port, char const* ip,
                     unsigned int ipaddr) const;
    int SendToServer(char* buf, int len, unsigned short port, char const* ip) const;
    void InetAddr(char const* ip) const;
    int m_sock;         // +0
    int m_clientSock;   // +4
};

int getErrno();

#endif
