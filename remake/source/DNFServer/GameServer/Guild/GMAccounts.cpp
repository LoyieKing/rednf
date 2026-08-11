// df_guild_r — GMAccounts (split from source/guild per ORIG layout)
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

#include "GMAccounts.h"
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
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

namespace WongWork
{
bool CGMAccounts::stGMInfo_t::operator==(const stGMInfo_t& other) const
{
    return m_field0 == other.m_field0;
}

void CGMAccounts::LoadGmList(unsigned int group, int index)
{
    stGMInfo_t info;
    info.m_field0 = group;
    info.m_field1 = index;
    m_list.push_back(info);
}

void CGMAccounts::clearGmList()
{
    m_list.clear();
}

void CGMAccounts::AppendGM_Sys(unsigned int id, char flag)
{
    stGMInfo_t info;
    info.m_field0 = id;
    info.m_field1 = (unsigned int)flag;
    m_list.push_back(info);
    char* mid = NumberToString(id, 0);
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0xcd, "./log/Init", "GM List Add mid:%s", mid);
}

int CGMAccounts::loadGMAccounts(const char* path)
{
    return 1;
}

int CGMAccounts::isGM(unsigned int id)
{
    stGMInfo_t key;
    key.m_field1 = 3;
    key.m_field0 = id;
    std::list<stGMInfo_t>::iterator it = std::find(m_list.begin(), m_list.end(), key);
    return it != m_list.end();
}

int CGMAccounts::appendGM(unsigned int id, unsigned int value)
{
    char local[0x10];
    (void)local;
    (void)id;
    (void)value;
    return 0;
}

int CGMAccounts::removeGM(unsigned int id, unsigned int value)
{
    char local[0x20];
    (void)local;
    (void)id;
    (void)value;
    return 0;
}

CGMAccounts::stGMInfo_t CGMAccounts::getGMInfo(unsigned int id) const
{
    stGMInfo_t key;
    key.m_field0 = id;
    key.m_field1 = 3;
    stGMInfo_t result;
    result.m_field0 = 0;
    result.m_field1 = 3;
    std::list<stGMInfo_t>::const_iterator it = std::find(m_list.begin(), m_list.end(), key);
    if (it != m_list.end())
    {
        result = *it;
    }
    return result;
}
}
