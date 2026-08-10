// df_dbmw_r - ServerLoadChecker (ORIG ServerLoadChecker.cpp)
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
int get_day_interval(struct tm* a, struct tm* b);

// ---- CGuildManager / WongWork ----
int getNotiPacketNameCount()
{
    return 0x233;
}
int getCmdPacketNameCount()
{
    return 0x25f;
}
CAppLoadChecker::CAppLoadChecker()
{
    m_tcpRecvLast = 0;
    m_udpRecvLast = 0;
    m_tcpSendLast = 0;
    m_tcpRecvLevel = 0;
    m_udpRecvLevel = 0;
    m_tcpSendLevel = 0;
}
bool CAppLoadChecker::CheckTcpRecvQ(int size)
{
    if (checkTcpRecvLoad(size))
    {
        setTcpRecvQueue(size);
        return 1;
    }
    return 0;
}
bool CAppLoadChecker::CheckUdpRecvQ(int size)
{
    if (checkUdpRecvLoad(size))
    {
        setUdpRecvQueue(size);
        return 1;
    }
    return 0;
}
bool CAppLoadChecker::CheckTcpSendQ(int size)
{
    if (checkTcpSendLoad(size))
    {
        setTcpSendQueue(size);
        return 1;
    }
    return 0;
}
void CAppLoadChecker::setTcpRecvQueue(int size) { m_tcpRecvLast = size; }
void CAppLoadChecker::setUdpRecvQueue(int size) { m_udpRecvLast = size; }
void CAppLoadChecker::setTcpSendQueue(int size) { m_tcpSendLast = size; }
void CAppLoadChecker::RequestDB(CServerHandler* serverHandler, int flag, int size)
{
    Packet_Server_Queue_Load_Statistic pkt;
    pkt.m_fieldA = 0xc8;
    pkt.m_fieldB = (char)flag;
    pkt.m_fieldC = (unsigned short)size;
    CPacketTranslater::OnServeQueueLoadStatistic(&pkt);
}
bool CAppLoadChecker::checkTcpRecvLoad(int size)
{
    if (m_tcpRecvLevel < 1 && 0x32 < size - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 1;
        return 1;
    }
    if (m_tcpRecvLevel < 2 && 0x64 < size - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 2;
        return 1;
    }
    if (m_tcpRecvLevel < 3 && 0xc8 < size - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 3;
        return 1;
    }
    if (m_tcpRecvLevel < 4 && 0x1f4 < size - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 4;
        return 1;
    }
    if (m_tcpRecvLevel < 5 && 0x3e8 < size - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 5;
        return 1;
    }
    if (m_tcpRecvLevel < 6 && 0x1388 < size - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 6;
        return 1;
    }
    if (m_tcpRecvLevel == 6 && 0x1388 < size - m_tcpRecvLast)
        return 1;
    if (m_tcpRecvLevel >= 0 && 0x32 < m_tcpRecvLast - size)
    {
        m_tcpRecvLevel = 0xff;
        return 1;
    }
    if (m_tcpRecvLevel >= (char)0xff && 0x64 < m_tcpRecvLast - size)
    {
        m_tcpRecvLevel = 0xfe;
        return 1;
    }
    if (m_tcpRecvLevel >= (char)0xfe && 0xc8 < m_tcpRecvLast - size)
    {
        m_tcpRecvLevel = 0xfd;
        return 1;
    }
    if (m_tcpRecvLevel >= (char)0xfd && 0x1f4 < m_tcpRecvLast - size)
    {
        m_tcpRecvLevel = 0xfc;
        return 1;
    }
    if (m_tcpRecvLevel >= (char)0xfc && 0x3e8 < m_tcpRecvLast - size)
    {
        m_tcpRecvLevel = 0xfb;
        return 1;
    }
    if (m_tcpRecvLevel >= (char)0xfb && 0x1388 < m_tcpRecvLast - size)
    {
        m_tcpRecvLevel = 0xfa;
        return 1;
    }
    if (m_tcpRecvLevel == (char)0xfa && 0x1388 < m_tcpRecvLast - size)
        return 1;
    return 0;
}
bool CAppLoadChecker::checkUdpRecvLoad(int size)
{
    if (m_udpRecvLevel < 1 && 0x32 < size - m_udpRecvLast)
    {
        m_udpRecvLevel = 1;
        return 1;
    }
    if (m_udpRecvLevel < 2 && 0x64 < size - m_udpRecvLast)
    {
        m_udpRecvLevel = 2;
        return 1;
    }
    if (m_udpRecvLevel < 3 && 0xc8 < size - m_udpRecvLast)
    {
        m_udpRecvLevel = 3;
        return 1;
    }
    if (m_udpRecvLevel < 4 && 0x1f4 < size - m_udpRecvLast)
    {
        m_udpRecvLevel = 4;
        return 1;
    }
    if (m_udpRecvLevel < 5 && 0x3e8 < size - m_udpRecvLast)
    {
        m_udpRecvLevel = 5;
        return 1;
    }
    if (m_udpRecvLevel < 6 && 0x1388 < size - m_udpRecvLast)
    {
        m_udpRecvLevel = 6;
        return 1;
    }
    if (m_udpRecvLevel == 6 && 0x1388 < size - m_udpRecvLast)
        return 1;
    if (m_udpRecvLevel >= 0 && 0x32 < m_udpRecvLast - size)
    {
        m_udpRecvLevel = 0xff;
        return 1;
    }
    if (m_udpRecvLevel >= (char)0xff && 0x64 < m_udpRecvLast - size)
    {
        m_udpRecvLevel = 0xfe;
        return 1;
    }
    if (m_udpRecvLevel >= (char)0xfe && 0xc8 < m_udpRecvLast - size)
    {
        m_udpRecvLevel = 0xfd;
        return 1;
    }
    if (m_udpRecvLevel >= (char)0xfd && 0x1f4 < m_udpRecvLast - size)
    {
        m_udpRecvLevel = 0xfc;
        return 1;
    }
    if (m_udpRecvLevel >= (char)0xfc && 0x3e8 < m_udpRecvLast - size)
    {
        m_udpRecvLevel = 0xfb;
        return 1;
    }
    if (m_udpRecvLevel >= (char)0xfb && 0x1388 < m_udpRecvLast - size)
    {
        m_udpRecvLevel = 0xfa;
        return 1;
    }
    if (m_udpRecvLevel == (char)0xfa && 0x1388 < m_udpRecvLast - size)
        return 1;
    return 0;
}
bool CAppLoadChecker::checkTcpSendLoad(int size)
{
    if (m_tcpSendLevel < 1 && 0x32 < size - m_tcpSendLast)
    {
        m_tcpSendLevel = 1;
        return 1;
    }
    if (m_tcpSendLevel < 2 && 0x64 < size - m_tcpSendLast)
    {
        m_tcpSendLevel = 2;
        return 1;
    }
    if (m_tcpSendLevel < 3 && 0xc8 < size - m_tcpSendLast)
    {
        m_tcpSendLevel = 3;
        return 1;
    }
    if (m_tcpSendLevel < 4 && 0x1f4 < size - m_tcpSendLast)
    {
        m_tcpSendLevel = 4;
        return 1;
    }
    if (m_tcpSendLevel < 5 && 0x3e8 < size - m_tcpSendLast)
    {
        m_tcpSendLevel = 5;
        return 1;
    }
    if (m_tcpSendLevel < 6 && 0x1388 < size - m_tcpSendLast)
    {
        m_tcpSendLevel = 6;
        return 1;
    }
    if (m_tcpSendLevel == 6 && 0x1388 < size - m_tcpSendLast)
        return 1;
    if (m_tcpSendLevel >= 0 && 0x32 < m_tcpSendLast - size)
    {
        m_udpRecvLevel = 0xff;   // 原版怪癖：此处写 +0xd（udp 等级）
        return 1;
    }
    if (m_tcpSendLevel >= (char)0xff && 0x64 < m_tcpSendLast - size)
    {
        m_tcpSendLevel = 0xfe;
        return 1;
    }
    if (m_tcpSendLevel >= (char)0xfe && 0xc8 < m_tcpSendLast - size)
    {
        m_tcpSendLevel = 0xfd;
        return 1;
    }
    if (m_tcpSendLevel >= (char)0xfd && 0x1f4 < m_tcpSendLast - size)
    {
        m_tcpSendLevel = 0xfc;
        return 1;
    }
    if (m_tcpSendLevel >= (char)0xfc && 0x3e8 < m_tcpSendLast - size)
    {
        m_tcpSendLevel = 0xfb;
        return 1;
    }
    if (m_tcpSendLevel >= (char)0xfb && 0x1388 < m_tcpSendLast - size)
    {
        m_tcpSendLevel = 0xfa;
        return 1;
    }
    if (m_tcpSendLevel == (char)0xfa && 0x1388 < m_tcpSendLast - size)
        return 1;
    return 0;
}
CAppLoadChecker* CAppLoadCheckerInstance()
{
    static CAppLoadChecker instance;
    return &instance;
}
