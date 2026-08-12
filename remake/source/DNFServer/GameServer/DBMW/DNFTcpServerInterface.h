#ifndef DBMW_DNFTCPSERVERINTERFACE_H_
#define DBMW_DNFTCPSERVERINTERFACE_H_

#include "DBMWCommon.h"


class CTcpServer
{
public:
    CTcpServer();
    ~CTcpServer();
    void Init(unsigned int sock, CTcpNetSystem* net);
    void SetServerType(unsigned char type);
    unsigned char GetServerType();
    void* GetSocket();
    bool IsValidServer();
    void NotifyHeartbeat();
    char IsHeartbeatTimeOver();
    void SendToServer(char* buf);
    char* makePacketHeader(unsigned short type, unsigned short size);
    unsigned char m_index;  // +0
    char m_pad1[3];
    unsigned int m_socket;  // +4（原版为无符号整型套接字句柄）
    CTcpNetSystem* m_net;  // +8
    time_t m_heartbeat; // +0xc
};

#endif  // DBMW_DNFTCPSERVERINTERFACE_H_
