#ifndef DNF_TCP_SERVER_INTERFACE_H_
#define DNF_TCP_SERVER_INTERFACE_H_

#include <sys/types.h>

class CTcpNetSystem;

// ---- CTcpServer：0x10 ----
class CTcpServer
{
public:
    CTcpServer();
    ~CTcpServer();
    void Init(unsigned int sock, CTcpNetSystem* net);
    void SetServerIndex(unsigned char idx);
    unsigned char GetServerIndex();
    void* GetSocket();
    bool IsValidServer();
    void NotifyHeartbeat();
    char IsHeartbeatTimeOver();
    void SendToServer(char* buf);
    unsigned short makePacketHeader(unsigned short type, unsigned short size);
    unsigned char m_index;  // +0
    char m_pad1[3];
    unsigned int m_socket;  // +4（原版为无符号整型套接字句柄）
    CTcpNetSystem* m_net;  // +8
    time_t m_heartbeat; // +0xc
};

#endif  // DNF_TCP_SERVER_INTERFACE_H_
