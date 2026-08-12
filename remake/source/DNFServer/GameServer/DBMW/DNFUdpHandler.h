#ifndef DBMW_DNFUDPHANDLER_H_
#define DBMW_DNFUDPHANDLER_H_

#include "DBMWCommon.h"


class CUdpHandler
{
public:
    CUdpHandler();
    unsigned int InetAddr(const char* ip) const;
    int InitServerSocket(int port);
    int InitClientSocket();
    bool RecvFromClient(char* buf, int* size, unsigned int* addr, unsigned short* port) const;
    int SendToClient(char* buf, int len, unsigned short port, const char* ip,
                     unsigned int addr) const;
    int SendToServer(char* buf, int len, unsigned short port, const char* ip) const;
    char RecvFromServer(char* buf, int* size, unsigned int* addr, unsigned short* port) const;
    int GetServerSocket();
    int m_sock;         // +0
    int m_clientSock;   // +4
};

#endif  // DBMW_DNFUDPHANDLER_H_
