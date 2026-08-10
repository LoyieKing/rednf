#ifndef DBMW_DNFPROHIBITUSER_H_
#define DBMW_DNFPROHIBITUSER_H_

#include "DBMWCommon.h"


class CDNFProhibitUser
{
public:
    CDNFProhibitUser();
    ~CDNFProhibitUser();
    static void* operator new(unsigned int size);
    static void operator delete(void* ptr);
    static void operator delete(void* ptr, unsigned int size);
    unsigned int GetDBID();
    unsigned short GetProhibitRemainTime();
    unsigned char GetMonitorRetPacketCnt();
    char GetConnectFlag();
    void SetIpPort(unsigned int ip, unsigned short port);
    void GetIpPort(unsigned int& ip, unsigned short& port);
    void SetMonitorWaitTime(unsigned int dbid, short time);
    void SetProhibitUserInfo(char flag);
    void IncreMonitorRetPacket();
    char IsTimeOutWaitMonitor();
    unsigned int m_dbid;        // +0
    unsigned short m_remainTime;  // +4
    unsigned char m_retPacketCnt; // +6
    char m_pad7;                // +7
    unsigned int m_ip;          // +8
    unsigned short m_port;      // +0xc
    char m_connectFlag;         // +0xe
    char m_padF;                // +0xf
};

#endif  // DBMW_DNFPROHIBITUSER_H_
