// df_guild_r — Scheduler (split from source/guild per ORIG layout)
#include <string.h>
#include <time.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include "tinyxml.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel_Next.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel_Next.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "Scheduler.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
#include "DNFGuild.h"
#include "DNFGuildManager.h"
#include "DNFGuildServerMain.h"
#include "DNFGuildWar.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMonitorServer.h"
#include "DNFNetworkThread.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFPowerWarConfig.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFSignal.h"
#include "DNFSignalTranslator.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
#include "MemPool.h"
#include "MemoryCashManager.h"
#include "PacketCounter.h"
#include "Peer.h"
#include "Power.h"
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

void CScheduler::SetSpecificDayScheduleHour(int day, int hour)
{
    m_table[day].m_hour = (char)hour;
}

CScheduler::CScheduler()
{
    m_day = 0xff;
    m_min = 0xff;
    m_hour = 0xff;
    m_sec = 0xff;
    m_week = 0xffff;
    m_flag1 = 0xff;
    m_flag2 = 0xff;
}

CScheduler::~CScheduler()
{
}

void CScheduler::Clear()
{
    m_day = 0xff;
    m_min = 0xff;
    m_hour = 0xff;
    m_sec = 0xff;
    m_week = 0xffff;
    m_flag1 = 0xff;
    m_flag2 = 0xff;
    for (int i = 0; i < 7; i++)
    {
        m_table[i].m_flag = 0;
        m_table[i].m_hour = 0xff;
        m_table[i].m_min = 0xff;
        m_table[i].m_reserved = 0xff;
    }
}

void CScheduler::SetSpecialHour(int hour)
{
    m_hour = (char)hour;
    m_min = 0;
}

int CScheduler::IsOnTimeSpecialHour(int hour, int min)
{
    if ((char)m_hour == hour && (char)m_min == min)
        return 1;
    return 0;
}

void CScheduler::SetSpecialDayHour(int day, int hour)
{
    m_hour = (char)hour;
    m_sec = (char)day;
    m_min = 0;
}

int CScheduler::IsOnTimeSpecialDayHour(int day, int hour, int min)
{
    if ((char)m_sec == day && (char)m_hour == hour && (char)m_min == min)
        return 1;
    return 0;
}

int CScheduler::GetSpecificDayScheduleHour(int day)
{
    return ((int)(char)m_table[day].m_min - (int)(char)m_table[day].m_hour) * 0x3c;
}

void CScheduler::SetSpecialWeekDayHour(std::vector<STPowerWarScheduleTime> schedule)
{
    for (std::vector<STPowerWarScheduleTime>::iterator it = schedule.begin();
         it != schedule.end(); ++it)
    {
        STPowerWarScheduleTime* p = &(*it);
        m_table[p->m_data[0]].m_flag = 1;
        m_table[p->m_data[0]].m_hour = p->m_data[1];
        m_table[p->m_data[0]].m_min = p->m_data[2];
        m_table[p->m_data[0]].m_reserved = 0;
    }
}

void CScheduler::SetSpecialWeekDayHour(int day, int hour)
{
    m_hour = (char)hour;
    m_flag1 = (char)day;
    m_min = 0;
}

STPowerWarScheduleTime* CScheduler::GetNextScheduleTime(unsigned char& hour,
                                                       unsigned char& min)
{
    tm t;
    time_t now;
    time_t next;
    time(&now);
    t = *localtime(&now);
    if (m_table[t.tm_wday].m_flag != 0 &&
        t.tm_hour <= (char)m_table[t.tm_wday].m_hour)
    {
        hour = (unsigned char)m_table[t.tm_wday].m_hour;
        min = (unsigned char)m_table[t.tm_wday].m_min;
        return (STPowerWarScheduleTime*)localtime(&now);
    }
    bool found = false;
    int target = 0;
    int daysAhead = 0;
    int d = t.tm_wday + 1;
    for (; d <= 6; d++)
    {
        if (m_table[d].m_flag != 0)
        {
            found = true;
            target = d;
            break;
        }
    }
    if (found)
    {
        daysAhead = target - t.tm_wday;
    }
    else
    {
        int i = 0;
        for (; i <= 6; i++)
        {
            if (m_table[i].m_flag != 0)
            {
                target = i;
                break;
            }
        }
        daysAhead = 7 - t.tm_wday + target;
    }
    next = mktime(&t);
    next += daysAhead * 0x15180;
    hour = (unsigned char)m_table[target].m_hour;
    min = (unsigned char)m_table[target].m_min;
    return (STPowerWarScheduleTime*)localtime(&next);
}

int CScheduler::IsOnTimeSpecialWeekDayHour(int day, int hour, int min)
{
    if (m_table[day].m_flag != 0 && (char)m_table[day].m_hour == hour &&
        (char)m_table[day].m_reserved == min)
        return 1;
    return 0;
}

bool CheckDailyScheduleTimeOver(int hour, long t)
{
    time_t now;
    time(&now);
    struct tm local = *localtime(&now);
    struct tm local2 = local;
    local2.tm_hour = hour;
    local2.tm_min = 0;
    local2.tm_sec = 0;
    time_t target = mktime(&local2);
    if (local.tm_hour < hour)
    {
        target -= 86400;
    }
    return t < target;
}

bool CheckDayHourScheduleTimeOver(int day, int hour, long t)
{
    time_t now;
    time(&now);
    struct tm local = *localtime(&now);
    struct tm local2 = local;
    local2.tm_hour = hour;
    local2.tm_min = 0;
    local2.tm_sec = 0;
    time_t target = mktime(&local2);
    if (local.tm_hour < hour)
    {
        target -= 86400;
    }
    return t < target + (1 - day) * 86400;
}

bool CheckDayScheduleTimeOver(int hour, long t)
{
    time_t now;
    time(&now);
    return t < now - hour * 86400;
}

void GetScheduleTimeAsWDay(int day, int hour)
{
    time_t now;
    time(&now);
    struct tm local = *localtime(&now);
    int diff = day - local.tm_wday;
    if (diff < 0 || (diff == 0 && hour <= local.tm_hour))
    {
        diff += 7;
    }
    time_t target = mktime(&local);
    target = diff * 86400 + target;
    localtime(&target);
}
