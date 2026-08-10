#ifndef DNF_GAME_SERVER_H_
#define DNF_GAME_SERVER_H_

#include <string>

class CUdpHandler;

// CGameServer：0x14
// group@0(0xff=无效) / name string@4 / port@8 / connectFlag@0xa /
// heartBeatCount@0xb(0x1e) / heartBeatOver@0xc / channelNo@0xd / udp@0x10
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
    unsigned char GetChannelNo();
    unsigned char GetGroupNo();
    unsigned char m_group;          // +0（0xff=无效）
    std::string m_name;             // +4
    unsigned short m_port;          // +8
    unsigned char m_connectFlag;    // +0xa
    unsigned char m_heartBeatCount; // +0xb（0x1e）
    unsigned char m_heartBeatOver;  // +0xc
    unsigned char m_channelNo;      // +0xd
    CUdpHandler* m_udp;             // +0x10
};

#endif // DNF_GAME_SERVER_H_
