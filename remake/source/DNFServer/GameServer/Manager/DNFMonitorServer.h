#ifndef DNF_MONITOR_SERVER_H_
#define DNF_MONITOR_SERVER_H_

#include <string>

class CUdpHandler;

// ---- CMonitorServer：0x14 ----
class CMonitorServer
{
public:
    CMonitorServer();
    ~CMonitorServer();
    void ResetHeartBeat();
    bool IsConnected();
    void SetConnFlag(bool flag);
    void OnDisconnect();
    bool IsValidMonitorServer();
    bool IsHeartBeatTimeOver();
    void Init(std::string& name, unsigned short port, unsigned char flag);
    void SendToServer(char* buf, int len);
    unsigned char m_index;  // +0
    char m_valid;       // +1
    char m_pad2[2];     // +2
    std::string m_name; // +4
    unsigned short m_port;  // +8
    bool m_connected;   // +0xa
    char m_heartBeat;   // +0xb
    unsigned char m_fieldC;      // +0xc（ORIG seta 无符号比较）
    char m_padD[3];     // +0xd
    CUdpHandler* m_udpHandler;  // +0x10
};

#endif  // DNF_MONITOR_SERVER_H_
