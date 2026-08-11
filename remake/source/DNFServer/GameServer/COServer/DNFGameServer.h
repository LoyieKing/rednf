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
    // ORIG 实测：返回类型为 bool（调用方 test %al,%al 直用低字节；
    // int 返回会让 4.4.6 调用方产生 setne 物化）。函数体 0/1 形态不变。
    bool IsHeartBeatTimeOver();
    void ResetHeartBeat();
    void SendToGameServer(char* buf, int len);
    void OnDisconnect();
    bool IsConnected();
    void SetConnFlag(bool flag);
    unsigned char GetChannelNo();
    unsigned char GetGroupNo();
    unsigned char m_group;          // +0（0xff=无效）
    std::string m_name;             // +4
    unsigned short m_port;          // +8
    bool m_connectFlag;             // +0xa
    unsigned char m_heartBeatCount; // +0xb（0x1e）
    unsigned char m_heartBeatOver;  // +0xc
    unsigned char m_channelNo;      // +0xd
    CUdpHandler* m_udp;             // +0x10
};

#endif // DNF_GAME_SERVER_H_
