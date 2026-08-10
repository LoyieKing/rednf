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
    *(char*)((char*)this + day * 4 + 9) = (char)hour;
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
        *(char*)((char*)this + i * 4 + 8) = 0;
        *(char*)((char*)this + i * 4 + 9) = 0xff;
        *(char*)((char*)this + i * 4 + 10) = 0xff;
        *(char*)((char*)this + i * 4 + 0xb) = 0xff;
    }
}

void CScheduler::SetSpecialHour(int hour)
{
    *(char*)((char*)this + 2) = (char)hour;
    *(char*)((char*)this + 1) = 0;
}

int CScheduler::IsOnTimeSpecialHour(int hour, int min)
{
    return (char)*(char*)((char*)this + 2) == hour &&
           (char)*(char*)((char*)this + 1) == min;
}

void CScheduler::SetSpecialDayHour(int day, int hour)
{
    *(char*)((char*)this + 2) = (char)hour;
    *(char*)((char*)this + 3) = (char)day;
    *(char*)((char*)this + 1) = 0;
}

int CScheduler::IsOnTimeSpecialDayHour(int day, int hour, int min)
{
    return (char)*(char*)((char*)this + 3) == day &&
           (char)*(char*)((char*)this + 2) == hour &&
           (char)*(char*)((char*)this + 1) == min;
}

int CScheduler::GetSpecificDayScheduleHour(int day)
{
    return ((int)(char)*(char*)((char*)this + day * 4 + 10) -
            (int)(char)*(char*)((char*)this + day * 4 + 9)) * 0x3c;
}

void CScheduler::SetSpecialWeekDayHour(std::vector<STPowerWarScheduleTime> schedule)
{
    for (std::vector<STPowerWarScheduleTime>::iterator it = schedule.begin();
         it != schedule.end(); ++it)
    {
        char* p = (char*)&(*it);
        *(char*)((char*)this + p[0] * 4 + 8) = 1;
        *(char*)((char*)this + p[0] * 4 + 9) = p[1];
        *(char*)((char*)this + p[0] * 4 + 10) = p[2];
        *(char*)((char*)this + p[0] * 4 + 0xb) = 0;
    }
}

void CScheduler::SetSpecialWeekDayHour(int day, int hour)
{
    *(char*)((char*)this + day * 4 + 8) = 1;
    *(char*)((char*)this + day * 4 + 9) = (char)hour;
}

STPowerWarScheduleTime* CScheduler::GetNextScheduleTime(unsigned char& hour,
                                                       unsigned char& min)
{
    time_t now = time(0);
    tm* pt = localtime(&now);
    tm t;
    t.tm_sec = pt->tm_sec;
    t.tm_min = pt->tm_min;
    t.tm_hour = pt->tm_hour;
    t.tm_mday = pt->tm_mday;
    t.tm_mon = pt->tm_mon;
    t.tm_year = pt->tm_year;
    t.tm_wday = pt->tm_wday;
    t.tm_yday = pt->tm_yday;
    t.tm_isdst = pt->tm_isdst;
    t.tm_gmtoff = pt->tm_gmtoff;
    t.tm_zone = pt->tm_zone;
    int curDay = pt->tm_wday;
    if (*(char*)((char*)this + curDay * 4 + 8) == 0 ||
        (char)*(char*)((char*)this + curDay * 4 + 9) < pt->tm_hour)
    {
        int target = -1;
        int d = curDay;
        do
        {
            d++;
            if (d > 6)
            {
                break;
            }
        } while (*(char*)((char*)this + d * 4 + 8) == 0);
        if (d <= 6)
        {
            target = d;
        }
        else
        {
            for (int i = 0; i < 7; i++)
            {
                if (*(char*)((char*)this + i * 4 + 8) != 0)
                {
                    target = i;
                    break;
                }
            }
        }
        int daysAhead = (d <= 6) ? (target - curDay) : ((7 - curDay) + target);
        time_t next = daysAhead * 0x15180 + mktime(&t);
        hour = (unsigned char)*(char*)((char*)this + target * 4 + 9);
        min = (unsigned char)*(char*)((char*)this + target * 4 + 10);
        return (STPowerWarScheduleTime*)localtime(&next);
    }
    else
    {
        hour = (unsigned char)*(char*)((char*)this + curDay * 4 + 9);
        min = (unsigned char)*(char*)((char*)this + curDay * 4 + 10);
        return (STPowerWarScheduleTime*)localtime(&now);
    }
}

int CScheduler::IsOnTimeSpecialWeekDayHour(int day, int hour, int min)
{
    if (*(char*)((char*)this + day * 4 + 8) != 0 &&
        *(char*)((char*)this + day * 4 + 9) == (char)hour &&
        *(char*)((char*)this + day * 4 + 0xb) == (char)min)
    {
        return 1;
    }
    return 0;
}

bool CheckDailyScheduleTimeOver(int hour, long t)
{
    time_t now = time(0);
    tm* pt = localtime(&now);
    tm local;
    local.tm_sec = pt->tm_sec;
    local.tm_min = pt->tm_min;
    local.tm_hour = pt->tm_hour;
    local.tm_mday = pt->tm_mday;
    local.tm_mon = pt->tm_mon;
    local.tm_year = pt->tm_year;
    local.tm_wday = pt->tm_wday;
    local.tm_yday = pt->tm_yday;
    local.tm_isdst = pt->tm_isdst;
    local.tm_gmtoff = pt->tm_gmtoff;
    local.tm_zone = pt->tm_zone;
    local.tm_hour = hour;
    local.tm_min = 0;
    local.tm_sec = 0;
    time_t target = mktime(&local);
    return t < target;
}

int CheckDayHourScheduleTimeOver(int day, int hour, long t)
{
    time_t now = time(0);
    tm* pt = localtime(&now);
    tm local;
    local.tm_sec = pt->tm_sec;
    local.tm_min = pt->tm_min;
    local.tm_hour = pt->tm_hour;
    local.tm_mday = pt->tm_mday;
    local.tm_mon = pt->tm_mon;
    local.tm_year = pt->tm_year;
    local.tm_wday = pt->tm_wday;
    local.tm_yday = pt->tm_yday;
    local.tm_isdst = pt->tm_isdst;
    local.tm_gmtoff = pt->tm_gmtoff;
    local.tm_zone = pt->tm_zone;
    local.tm_hour = hour;
    local.tm_min = 0;
    local.tm_sec = 0;
    time_t target = mktime(&local);
    return t < target;
}

int CheckDayScheduleTimeOver(int hour, long t)
{
    time_t now = time(0);
    time_t target = (time_t)((long)now - (long)hour * 86400);
    return t < target;
}

void GetScheduleTimeAsWDay(int day, int hour)
{
    time_t now = time(0);
    tm* pt = localtime(&now);
    tm local;
    local.tm_sec = pt->tm_sec;
    local.tm_min = pt->tm_min;
    local.tm_hour = pt->tm_hour;
    local.tm_mday = pt->tm_mday;
    local.tm_mon = pt->tm_mon;
    local.tm_year = pt->tm_year;
    local.tm_wday = pt->tm_wday;
    local.tm_yday = pt->tm_yday;
    local.tm_isdst = pt->tm_isdst;
    local.tm_gmtoff = pt->tm_gmtoff;
    local.tm_zone = pt->tm_zone;
    int diff = day - local.tm_wday;
    if (diff < 0 || (diff == 0 && hour <= local.tm_hour))
    {
        diff += 7;
    }
    time_t t2 = mktime(&local);
    time_t target = (time_t)(diff * 86400 + t2);
    localtime(&target);
}

