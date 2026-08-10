#ifndef DNF_UDP_HANDLER_H_
#define DNF_UDP_HANDLER_H_

#include "PacketHeader.h"

// ---- 包结构（coserver 专用，与 UDP 收发处理同文件）----
class Packet_CutOff_UDP_Call_UserInfo : public PacketHeader
{
public:
    Packet_CutOff_UDP_Call_UserInfo();
};

class Packet_Monitor_UDP_User_Getout : public PacketHeader
{
public:
    Packet_Monitor_UDP_User_Getout();
    unsigned int m_userID;  // +0xa
} __attribute__((packed));

// CUdpHandler：sock@0 / clientSock@4
class CUdpHandler
{
public:
    CUdpHandler();
    int InitServerSocket(int port);
    int InitClientSocket();
    int RecvFromClient(char* buf, int* len, unsigned int* ip, unsigned short* port) const;
    int SendToClient(char* buf, int len, unsigned short port, char const* ip,
                     unsigned int ipaddr) const;
    int RecvFromServer(char* buf, int* len, unsigned int* ip, unsigned short* port) const;
    int SendToServer(char* buf, int len, unsigned short port, char const* ip) const;
    void InetAddr(char const* ip) const;
    int m_sock;        // +0
    int m_clientSock;  // +4
};

#endif // DNF_UDP_HANDLER_H_
