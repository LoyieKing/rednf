// df_manager_r — CDNFProhibitUser（ORIG ProhibitUser.cpp）
#include "ProhibitUser.h"

#include "DNFPacketBuffer.h"

CDNFProhibitUser::CDNFProhibitUser()
{
    m_dbid = 0xffffffff;
    m_remainTime = 0;
    m_retPacketCnt = 0;
    m_ip = 0;
    m_port = 0;
    m_connectFlag = 0;
}

CDNFProhibitUser::~CDNFProhibitUser()
{
    m_remainTime = 0;
    m_dbid = 0xffffffff;
    m_retPacketCnt = 0;
    m_ip = 0;
    m_port = 0;
    m_connectFlag = 0;
}

void* CDNFProhibitUser::operator new(unsigned int size) { return g_prohibitUserPool.alloc(); }
void CDNFProhibitUser::operator delete(void* ptr) { g_prohibitUserPool.free(ptr); }
void CDNFProhibitUser::operator delete(void* ptr, unsigned int size) { g_prohibitUserPool.free(ptr, size); }

void CDNFProhibitUser::SetIpPort(unsigned int ip, unsigned short port)
{
    m_ip = ip;
    m_port = port;
}

void CDNFProhibitUser::GetIpPort(unsigned int& ip, unsigned short& port)
{
    ip = m_ip;
    port = m_port;
}

void CDNFProhibitUser::SetMonitorWaitTime(unsigned int dbid, short time)
{
    m_remainTime = time;
    m_dbid = dbid;
}

void CDNFProhibitUser::SetProhibitUserInfo(char flag)
{
    if (flag)
        m_connectFlag = flag;
}

unsigned int CDNFProhibitUser::GetDBID() { return m_dbid; }
short CDNFProhibitUser::GetProhibitRemainTime() { return m_remainTime; }
unsigned char CDNFProhibitUser::GetMonitorRetPacketCnt() { return m_retPacketCnt; }
char CDNFProhibitUser::GetConnectFlag() { return m_connectFlag; }
void CDNFProhibitUser::IncreMonitorRetPacket() { m_retPacketCnt++; }
char CDNFProhibitUser::IsTimeOutWaitMonitor()
{
    if (--m_remainTime <= 0)
        return 1;
    return 0;
}
