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

int CPowerWar::IsPowerWarOn()
{
    return *(int*)((char*)this + 4);
}

int CPowerWar::IsPowerWarOn() const
{
    return *(int*)((char*)this + 4);
}

unsigned short CPowerWar::getPowerWarEndKillPoint()
{
    return *(unsigned short*)((char*)this + 0xc);
}

unsigned short CPowerWar::getPowerWarEndKillPoint() const
{
    return *(unsigned short*)((char*)this + 0xc);
}

CPowerWar::CPowerWar()
{
    new (m_data) CEvent();
    *(int*)((char*)this + 8) = -1;
    *(unsigned short*)((char*)this + 0xc) = 0xffff;
    *(int*)((char*)this + 0x10) = 0;
    new ((char*)this + 0x14) CScheduler();
    *(CPowerWarConfig**)((char*)this + 0x10) = new CPowerWarConfig;
    resetEvent();
}

CPowerWar::~CPowerWar()
{
    if (*(CPowerWarConfig**)((char*)this + 0x10) != 0)
    {
        delete *(CPowerWarConfig**)((char*)this + 0x10);
        *(CPowerWarConfig**)((char*)this + 0x10) = 0;
    }
    puts("Power War Config Free Success!");
    ((CScheduler*)((char*)this + 0x14))->~CScheduler();
    ((CEvent*)m_data)->CEvent::~CEvent();
}

void CPowerWar::setEvent()
{
    *(unsigned char*)((char*)this + 4) = 1;
    time_t now = time(0);
    tm* t = localtime(&now);
    *(int*)((char*)this + 8) =
        ((CScheduler*)((char*)this + 0x14))->GetSpecificDayScheduleHour(t->tm_wday);
}

void CPowerWar::setProlongTime()
{
    *(unsigned char*)((char*)this + 4) = 1;
    *(int*)((char*)this + 8) += 10;
}

void CPowerWar::resetEvent()
{
    *(unsigned char*)((char*)this + 4) = 0;
    *(int*)((char*)this + 8) = -1;
    *(unsigned short*)((char*)this + 0xc) = 0xffff;
}

void CPowerWar::setPowerWarEndKillPoint(unsigned short point)
{
    *(unsigned short*)((char*)this + 0xc) = point;
}

int CPowerWar::ProcessByMinuteStartEvent()
{
    if (*(char*)((char*)this + 4) == 0)
    {
        time_t now = time(0);
        tm* pt = localtime(&now);
        int r = ((CScheduler*)((char*)this + 0x14))->IsOnTimeSpecialWeekDayHour(
            pt->tm_wday, pt->tm_hour, pt->tm_min);
        return r;
    }
    return 0;
}

int CPowerWar::ProcessByMinuteEndEvent()
{
    if (*(int*)((char*)this + 8) == -1 || *(char*)((char*)this + 4) == 0)
    {
        return -1;
    }
    *(int*)((char*)this + 8) -= 1;
    if (*(int*)((char*)this + 8) > 0)
    {
        return *(int*)((char*)this + 8);
    }
    return 0;
}

void CPowerWar::LoadPowerWarTableFile(char* path)
{
    DNF_LOG_SCOPE_LINE(0x9e, "./log/Power", "LoadPowerWarTableFile filename(%s)\n", path);
    CPowerWarConfig* config = *(CPowerWarConfig**)((char*)this + 0x10);
    config->Load_Table(std::string(path));
    ((CScheduler*)((char*)this + 0x14))->Clear();
    ST_PowerWarEventStartTimeConfig* info = config->GetInfo();
    std::vector<STPowerWarScheduleTime> schedule = *(std::vector<STPowerWarScheduleTime>*)((char*)info + 8);
    ((CScheduler*)((char*)this + 0x14))->SetSpecialWeekDayHour(schedule);
}

void CPowerWar::GetPowerWarConfigTbl(unsigned char& a, unsigned char& b, unsigned char& c,
                                     unsigned char& d)
{
    STPowerWarScheduleTime* p =
        (STPowerWarScheduleTime*)((CScheduler*)((char*)this + 0x14))
            ->GetNextScheduleTime(c, d);
    a = (unsigned char)(*(unsigned int*)((char*)p + 0x10) + 1);
    b = (unsigned char)*(unsigned int*)((char*)p + 0xc);
}

int CPowerWar::GetPowerWarRankingUpdateTime()
{
    if (*(int*)((char*)this + 0x10) == 0)
    {
        return 0;
    }
    return *(int*)((char*)((*(CPowerWarConfig**)((char*)this + 0x10))->GetInfo()) + 4);
}

ST_PowerWarEventStartTimeConfig::ST_PowerWarEventStartTimeConfig()
{
    m_day = 0xff;
    m_hour = 0xff;
    m_min = 0xff;
}

