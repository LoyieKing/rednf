#ifndef DNF_GAME_SERVER_H_
#define DNF_GAME_SERVER_H_

#include <string>

class CUdpHandler;

// CGameServer：0x14（同 coserver，无 GetChannelNo/GetGroupNo）
class CGameServer
{
public:
    CGameServer();
    ~CGameServer();
    void Init(unsigned char group, std::string& name, unsigned short port,
              unsigned char channel);
    bool IsValidServer();
    int IsHeartBeatTimeOver();
    void ResetHeartBeat();
    void SendToGameServer(char* buf, int len);
    void OnDisconnect();
    char IsConnected();
    void SetConnFlag(bool flag);
    unsigned char m_group;          // +0（0xff=无效）
    std::string m_name;             // +4
    unsigned short m_port;          // +8
    bool m_connectFlag;             // +0xa（coserver 同型：bool 使 SetConnFlag 恢复 ORIG 求值顺序）
    unsigned char m_heartBeatCount; // +0xb（0x1e）
    unsigned char m_heartBeatOver;  // +0xc
    unsigned char m_channelNo;      // +0xd
    CUdpHandler* m_udp;             // +0x10
};

#endif // DNF_GAME_SERVER_H_
