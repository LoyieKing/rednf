// df_guild_r — PowerWar (split from source/guild per ORIG layout)
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

#include "PowerWar.h"
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
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

ST_PowerWarEventStartTimeConfig::~ST_PowerWarEventStartTimeConfig()
{
}

bool CPowerWar::IsPowerWarOn() const
{
    return m_isEventOn != 0;
}

unsigned short CPowerWar::getPowerWarEndKillPoint()
{
    return m_endKillPoint;
}

unsigned short CPowerWar::getPowerWarEndKillPoint() const
{
    return m_endKillPoint;
}

CPowerWar::CPowerWar()
    : m_minuteCnt(-1), m_endKillPoint(0xffff), m_config(0)
{
    m_config = new CPowerWarConfig;
    resetEvent();
}

CPowerWar::~CPowerWar()
{
    if (m_config != 0)
    {
        delete m_config;
        m_config = 0;
    }
    puts("Power War Config Free Success!");
}

void CPowerWar::setEvent()
{
    m_isEventOn = 1;
    time_t now = time(0);
    tm* t = localtime(&now);
    m_minuteCnt = m_scheduler.GetSpecificDayScheduleHour(t->tm_wday);
}

void CPowerWar::setProlongTime()
{
    m_isEventOn = 1;
    m_minuteCnt = m_minuteCnt + 10;
}

void CPowerWar::resetEvent()
{
    m_isEventOn = 0;
    m_minuteCnt = -1;
    m_endKillPoint = 0xffff;
}

void CPowerWar::setPowerWarEndKillPoint(unsigned short point)
{
    if (m_endKillPoint == 0xffff)
    {
        m_endKillPoint = point;
    }
}

int CPowerWar::ProcessByMinuteStartEvent()
{
    if (m_isEventOn != 0)
    {
        return 0;
    }
    time_t now = time(0);
    tm* pt = localtime(&now);
    return m_scheduler.IsOnTimeSpecialWeekDayHour(pt->tm_wday, pt->tm_hour, pt->tm_min);
}

int CPowerWar::ProcessByMinuteEndEvent()
{
    if (m_minuteCnt == -1 || m_isEventOn == 0)
    {
        return -1;
    }
    m_minuteCnt = m_minuteCnt - 1;
    if (m_minuteCnt <= 0)
    {
        return 0;
    }
    return m_minuteCnt;
}

void CPowerWar::LoadPowerWarTableFile(char* path)
{
    DNF_LOG_SCOPE_LINE(0x9e, "./log/Power", "LoadPowerWarTableFile filename(%s)\n", path);
    m_config->Load_Table(std::string(path));
    m_scheduler.Clear();
    m_scheduler.SetSpecialWeekDayHour(
        *(std::vector<STPowerWarScheduleTime>*)((char*)m_config->GetInfo() + 8));
}

void CPowerWar::GetPowerWarConfigTbl(unsigned char& a, unsigned char& b, unsigned char& c,
                                     unsigned char& d)
{
    tm* p = (tm*)m_scheduler.GetNextScheduleTime(c, d);
    a = (unsigned char)(p->tm_mon + 1);
    b = (unsigned char)p->tm_mday;
}

int CPowerWar::GetPowerWarRankingUpdateTime()
{
    if (m_config != 0)
    {
        return m_config->GetInfo()->m_rankUpdateTime;
    }
    else
    {
        return 0;
    }
}

ST_PowerWarEventStartTimeConfig::ST_PowerWarEventStartTimeConfig()
    : m_day(0xff), m_hour(0xff), m_min(0xff)
{
}
