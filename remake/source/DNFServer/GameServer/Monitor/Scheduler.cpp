// df_monitor_r — Scheduler（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
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

#include "Scheduler.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

int GetNextSchedule(tm t, int wday, int hour, int min)
{
    int delta = wday - t.tm_wday;
    if (delta < 0)
    {
        delta += 7;
    }
    else if (delta == 0)
    {
        if (hour < t.tm_hour)
        {
            delta += 7;
        }
        else if (t.tm_hour == hour && min <= t.tm_min)
        {
            delta += 7;
        }
    }
    t.tm_hour = hour;
    t.tm_min = min;
    t.tm_sec = 0;
    int ret = (int)mktime(&t) + delta * 0x15180;
    return ret;
}

int GetPrevSchedule(tm t, int wday, int hour, int min)
{
    int delta = t.tm_wday - wday;
    if (delta < 0)
    {
        delta += 7;
    }
    t.tm_hour = hour;
    t.tm_min = min;
    t.tm_sec = 0;
    int ret = (int)mktime(&t) + delta * -0x15180;
    return ret;
}

bool CheckDailyScheduleTimeOver(int day, long t)
{
    time_t now;
    time(&now);
    tm t0 = *localtime(&now);
    tm target = t0;
    target.tm_hour = day;
    target.tm_min = 0;
    target.tm_sec = 0;
    time_t tt = mktime(&target);
    if (t0.tm_hour < day)
    {
        tt = tt - 0x15180;
    }
    return t < (long)tt;
}

bool CheckDayHourScheduleTimeOver(int day, int hour, long t)
{
    time_t now;
    time(&now);
    tm t0 = *localtime(&now);
    tm target = t0;
    target.tm_hour = hour;
    target.tm_min = 0;
    target.tm_sec = 0;
    time_t tt = mktime(&target);
    if (t0.tm_hour < hour)
    {
        tt = tt - 0x15180;
    }
    return t < (long)((1 - day) * 0x15180 + tt);
}

char CScheduler::IsOnTimeSpecialDayHour(int day, int hour, int min)
{
    if ((char)m_data[3] == day && (char)m_data[2] == hour && (char)m_data[1] == min)
    {
        return 1;
    }
    return 0;
}

CScheduler::CScheduler()
{
    m_data[0] = (char)0xff;
    m_data[1] = (char)0xff;
    m_data[2] = (char)0xff;
    m_data[3] = (char)0xff;
    m_ushort4 = (unsigned short)0xffff;
    m_data2[0] = (char)0xff;
    m_data2[1] = (char)0xff;
}

CScheduler::~CScheduler() {}

char CScheduler::IsOnTimeSpecialHour(int day, int hour)
{
    if ((char)m_data[2] == day && (char)m_data[1] == hour)
    {
        return 1;
    }
    return 0;
}

void CScheduler::SetSpecialHour(int hour)
{
    m_data[2] = (char)hour;
    m_data[1] = 0;
}

void CScheduler::SetSpecialDayHour(int day, int hour)
{
    m_data[2] = (char)hour;
    m_data[3] = (char)day;
    m_data[1] = 0;
}
