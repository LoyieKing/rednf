#ifndef DNF_MANAGER_SERVER_H_
#define DNF_MANAGER_SERVER_H_

#include <string>

class CUdpHandler;

// CManagerServer：0x10
class CManagerServer
{
public:
    CManagerServer();
    ~CManagerServer();
    void Init(unsigned char id, std::string& name, unsigned short port, unsigned char param);
    bool IsValidServer();
    int SendToServer(char* buf, int len);
    void OnDisconnect();
    void SendHeartBeat(int group);
    void SetConnFlag(bool flag);
    unsigned char m_id;          // +0（0xff=无效）
    std::string m_name;          // +4
    unsigned short m_port;       // +8
    unsigned char m_connectFlag; // +0xa
    unsigned char m_param;       // +0xb
    CUdpHandler* m_udp;          // +0xc
};

#endif // DNF_MANAGER_SERVER_H_
