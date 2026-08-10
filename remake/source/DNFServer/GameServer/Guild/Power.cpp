// df_guild_r — Power (split from source/guild per ORIG layout)
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

#include "Power.h"
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
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

CPower::CPower()
{
    InitPower();
}

CPower::~CPower()
{
    m_field4 = 0;
}

void CPower::InitPower()
{
    m_field4 = 0;
    m_characInfo.Initialize();
    m_guildInfo.Initialize();
}

void CPower::CleanPower()
{
    m_field4 = 0;
    m_characInfo.Clean();
    m_guildInfo.Clean();
}

void CPower::CalcPowerWarRank()
{
    m_guildInfo.CalcAllGuildRanking();
    m_characInfo.CalcAllUserRanking();
}

void CPower::RewardGuildPowerWarPoint(CGuildManager& gm, bool a, int b, int c, int d, int e)
{
    m_guildInfo.RewardGuildPowerWarPoint(gm, a, b, c, d, e);
}

void CPower::UpdatePowerWarInfo(int a, unsigned int b, unsigned int c)
{
    m_guildInfo.UpdateGuildPowerwarInfo(c, (unsigned short)a);
    m_characInfo.UpdatePowerwarCharacInfo(b, (unsigned short)a);
}

int CPower::IncScore(int score)
{
    m_field4 += score;
    if (m_field4 == 0x7fffffff)
    {
        m_field4 = 0x7fffffff;
    }
    return m_field4;
}

int CPower::GetScore()
{
    return m_field4;
}

void CPower::SetScore(int score)
{
    m_field4 = score;
}

CPowerWarGuildInfo* CPower::GetPowerWarGuildInfo()
{
    return &m_guildInfo;
}

CPowerWarCharacInfo* CPower::GetPowerWarCharacInfo()
{
    return &m_characInfo;
}

