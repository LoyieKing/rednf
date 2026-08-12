#ifndef PROHIBIT_USER_H_
#define PROHIBIT_USER_H_

// ---- CDNFProhibitUser：0x14 ----
class CDNFProhibitUser
{
public:
    CDNFProhibitUser();
    ~CDNFProhibitUser();
    static void* operator new(unsigned int size);
    static void operator delete(void* ptr);
    static void operator delete(void* ptr, unsigned int size);
    unsigned int GetDBID();
    short GetProhibitRemainTime();
    unsigned char GetMonitorRetPacketCnt();
    char GetConnectFlag();
    void SetIpPort(unsigned int ip, unsigned short port);
    void GetIpPort(unsigned int& ip, unsigned short& port);
    void SetMonitorWaitTime(unsigned int dbid, short time);
    void SetProhibitUserInfo(char flag);
    void IncreMonitorRetPacket();
    bool IsTimeOutWaitMonitor();
    unsigned int m_dbid;        // +0
    short m_remainTime;  // +4（ORIG setle 有符号比较）
    unsigned char m_retPacketCnt; // +6
    char m_pad7;                // +7
    unsigned int m_ip;          // +8
    unsigned short m_port;      // +0xc
    char m_connectFlag;         // +0xe
    char m_padF;                // +0xf
    int m_field10;              // +0x10（ORIG sizeof=0x14，MemPool 元素大小）
};

#endif  // PROHIBIT_USER_H_
