// df_monitor_r — SystemTimeHandler（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "SystemTimeHandler.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

void CommonTime::SetCurTime()
{
    time_t t;
    time(&t);
    tm* p = localtime(&t);
    register int v0 = p->tm_year - 100;
    register int v1 = p->tm_mon + 1;
    m_year = (unsigned char)v0;
    m_month = (unsigned char)v1;
    m_day = (unsigned char)(p->tm_mday);
    m_hour = (unsigned char)(p->tm_hour);
    m_minute = (unsigned char)(p->tm_min);
    m_wday = (unsigned char)(p->tm_wday);
}

CSystemTime::CSystemTime()
{
    gettimeofday((timeval*)&m_tv_sec, 0);
    m_sec = m_tv_sec;
    m_msec = m_tv_usec / 1000;
}

CSystemTimeHandler::CSystemTimeHandler() {}

CSystemTimeHandler* CSystemTimeHandlerInstance()
{
    static CSystemTimeHandler instance;
    return &instance;
}

unsigned int GetNowTime()
{
    return (unsigned int)time(0);
}
