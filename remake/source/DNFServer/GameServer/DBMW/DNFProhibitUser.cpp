// df_dbmw_r - DNFProhibitUser (ORIG DNFProhibitUser.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();
extern MemPool<CDNFProhibitUser> g_prohibitUserPool;

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
unsigned short CDNFProhibitUser::GetProhibitRemainTime() { return m_remainTime; }
unsigned char CDNFProhibitUser::GetMonitorRetPacketCnt() { return m_retPacketCnt; }
char CDNFProhibitUser::GetConnectFlag() { return m_connectFlag; }
void CDNFProhibitUser::IncreMonitorRetPacket() { m_retPacketCnt++; }
char CDNFProhibitUser::IsTimeOutWaitMonitor()
{
    m_remainTime--;
    if (m_remainTime <= 0)
        return 1;
    return 0;
}
void* CDNFProhibitUser::operator new(unsigned int size) { return g_prohibitUserPool.alloc(); }
void CDNFProhibitUser::operator delete(void* ptr) { g_prohibitUserPool.free(ptr); }
void CDNFProhibitUser::operator delete(void* ptr, unsigned int size) { g_prohibitUserPool.free(ptr, size); }
