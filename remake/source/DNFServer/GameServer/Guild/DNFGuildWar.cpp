// df_guild_r — DNFGuildWar (split from source/guild per ORIG layout)
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

#include "DNFGuildWar.h"
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
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

void CGuildWar::SetGuildWarEvent(bool flag, unsigned char param)
{
    m_bEventOn = flag;
    m_bParam = param;
}

bool CGuildWar::IsGuildWarEventOn()
{
    return m_bEventOn;
}

CGuildWar::CGuildWar()
{
    m_bEventOn = 0;
    m_bRankCnt = 0;
    m_bSaveCnt = 0;
    m_bParam = 0;
    m_bRankWorked = 0;
}

CGuildWar::~CGuildWar()
{
}

void CGuildWar::InitGuildWarInfo()
{
    m_bRankCnt = 0;
    m_bParam = 0;
    m_bEventOn = 0;
    Clear_VtGuildWarInfo();
}

bool CGuildWar::GetGuildWarInfoDBSave(unsigned int* a, unsigned int* b)
{
    if (a == 0 || b == 0)
    {
        return 0;
    }
    int i = 0;
    for (std::vector<std::pair<unsigned int, STGuildWarInfo*> >::iterator it =
             m_vtGuildWarInfo.begin(); it != m_vtGuildWarInfo.end(); ++it)
    {
        if (it->second != 0)
        {
            a[i] = it->second->m_guildKey;
            b[i] = it->second->m_point;
            i++;
        }
    }
    return 1;
}

void CGuildWar::GetGuildWarInfo(unsigned int* a, unsigned int* b, unsigned short* c)
{
    if (a == 0)
    {
        return;
    }
    if (b == 0)
    {
        return;
    }
    if (c == 0)
    {
        return;
    }
    int idx = 0;
    for (std::vector<std::pair<unsigned int, STGuildWarInfo*> >::iterator it =
             m_vtGuildWarInfo.begin(); it != m_vtGuildWarInfo.end(); ++it)
    {
        if (it->second != 0)
        {
            a[idx] = it->second->m_guildKey;
            b[idx] = it->second->m_point;
            c[idx] = (unsigned short)(idx + 1);
            idx++;
        }
    }
}

void CGuildWar::GetGuildWarInfo(ST_Guild_War_Rank_Info* info)
{
    if (info == 0)
    {
        return;
    }
    int count = 0;
    for (std::vector<std::pair<unsigned int, STGuildWarInfo*> >::iterator it =
             m_vtGuildWarInfo.begin(); it != m_vtGuildWarInfo.end(); ++it)
    {
        if (it->second != 0)
        {
            *(unsigned int*)((char*)info + count * 0x21 + 0) = it->second->m_guildKey;
            *(unsigned int*)((char*)info + count * 0x21 + 4) = it->second->m_point;
            memcpy((char*)info + count * 0x21 + 10, it->second->m_data, 0x16);
            count++;
            *(short*)((char*)info + count * 0x21 + 8) = (short)count;
        }
    }
}

void CGuildWar::Insert_GuildWarInfo(STGuildWarInfo* info)
{
    if (info == 0)
    {
        DNF_LOG_SCOPE_LINE(0x95, "./log/GuildWar", "[INSERT_ERR]info == 0\n");
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x90,"./log/GuildWar", "[INSERT]\tGuild Key : %d\tGuild Point : %d\n",
            info->m_guildKey, info->m_point);
        m_vtGuildWarInfo.push_back(std::make_pair(info->m_point, info));
    }
}

int CGuildWar::Find_GuildWarInfo(unsigned int guildId)
{
    if (m_vtGuildWarInfo.empty())
    {
        return 0;
    }
    for (std::vector<std::pair<unsigned int, STGuildWarInfo*> >::iterator it =
             m_vtGuildWarInfo.begin(); it != m_vtGuildWarInfo.end(); ++it)
    {
        if (it->second != 0 && it->second->m_guildKey == guildId)
        {
            return (int)it->second;
        }
    }
    return 0;
}

void CGuildWar::Clear_VtGuildWarInfo()
{
    if (m_vtGuildWarInfo.empty())
    {
        return;
    }
    for (std::vector<std::pair<unsigned int, STGuildWarInfo*> >::iterator it =
             m_vtGuildWarInfo.begin(); it != m_vtGuildWarInfo.end(); ++it)
    {
        delete it->second;
    }
    m_vtGuildWarInfo.clear();
}

bool GuildWarPairDataCompare(const std::pair<unsigned int, STGuildWarInfo*>& a,
                             const std::pair<unsigned int, STGuildWarInfo*>& b)
{
    return *(unsigned int*)((char*)b.second + 4) < *(unsigned int*)((char*)a.second + 4);
}

bool CGuildWar::Rank()
{
    if (m_vtGuildWarInfo.empty())
    {
        return 0;
    }
    if (m_vtGuildWarInfo.size() > 1)
    {
        std::sort(m_vtGuildWarInfo.begin(), m_vtGuildWarInfo.end(),
                  GuildWarPairDataCompare);
        printGuildWarRank();
        return 1;
    }
    return 0;
}

int CGuildWar::SameRankWork()
{
    if (m_vtGuildWarInfo.empty())
    {
        return 0;
    }
    std::vector<std::pair<unsigned int, STGuildWarInfo*> >::iterator it0 =
        m_vtGuildWarInfo.begin();
    STGuildWarInfo* first = it0->second;
    if (first == 0)
    {
        return 0;
    }
    int field4 = first->m_point;
    int count = 0;
    std::vector<std::pair<unsigned int, STGuildWarInfo*> >::iterator it =
        m_vtGuildWarInfo.begin();
    for (; it != m_vtGuildWarInfo.end(); ++it)
    {
        if ((*it).second == 0)
        {
            continue;
        }
        if (field4 != (*it).second->m_point)
        {
            break;
        }
        count++;
    }
    if (1 < count)
    {
        std::vector<std::pair<unsigned int, STGuildWarInfo*> >::iterator maxIt;
        std::vector<std::pair<unsigned int, STGuildWarInfo*> >::iterator it2;
        std::vector<std::pair<unsigned int, STGuildWarInfo*> >::iterator maxItTmp;
        it2 = m_vtGuildWarInfo.begin();
        maxIt = it2;
        if ((*maxIt).second == 0)
        {
            return 0;
        }
        unsigned int maxVal = (*maxIt).second->m_field[6];
        for (int i = 1; i < count; i++)
        {
            ++it2;
            if (maxVal < (*it2).second->m_field[6])
            {
                maxVal = (*it2).second->m_field[6];
                maxItTmp = it2;
            }
        }
        if ((*maxIt).second->m_field[6] != maxVal)
        {
            DNFFLib::Swap<STGuildWarInfo>((*maxIt).second, (*maxItTmp).second);
        }
    }
    return 1;
}

int CGuildWar::IsGuildWarEnterableGuild(unsigned int guildId)
{
    if (guildId == 0)
    {
        return 0;
    }
    if (Find_GuildWarInfo(guildId) == 0)
    {
        return 0;
    }
    return 1;
}

void CGuildWar::AddGuildWarPoint(unsigned int guildId, int point)
{
    if (guildId == 0)
    {
        return;
    }
    STGuildWarInfo* info = (STGuildWarInfo*)Find_GuildWarInfo(guildId);
    if (info == 0)
    {
        return;
    }
    info->m_point = info->m_point + point;
    m_bRankWorked = 1;
    printGuildWarRank();
}

void CGuildWar::RankProcess()
{
    if (!IsGuildWarEventOn())
    {
        return;
    }
    if (m_bRankWorked == 0)
    {
        return;
    }
    m_bRankCnt++;
    register bool bRanked = ((unsigned char)m_bRankCnt <= 1);
    if (bRanked)
    {
        return;
    }
    if (!Rank())
    {
        throw CDNFException(
            "CGuildWar::RankProcess : false == Rank() : May be m_vtGuildWarInfo is empty!");
    }
    m_bRankCnt = 0;
    m_bRankWorked = 0;
}

void CGuildWar::DBSaveProcess(CApplication* app)
{
    if (!IsGuildWarEventOn())
    {
        return;
    }
    m_bSaveCnt++;
    register bool bZero = (m_bSaveCnt == 0);
    if (bZero)
    {
        return;
    }
    Packet_Notice_DB_Save_Guild_War_Point pkt;
    if (GetGuildWarInfoDBSave((unsigned int*)((char*)&pkt + 0xb),
                              (unsigned int*)((char*)&pkt + 0x33)))
    {
        pkt.m_data[0] = app->Get_ServerGroup();
        app->Get_ServerHandler()->SendToDB(&pkt);
    }
    m_bSaveCnt = 0;
}

void CGuildWar::printGuildWarRank()
{
    if (m_vtGuildWarInfo.empty())
    {
        return;
    }
    int rank = 0;
    for (std::vector<std::pair<unsigned int, STGuildWarInfo*> >::iterator it =
             m_vtGuildWarInfo.begin(); it != m_vtGuildWarInfo.end(); ++it)
    {
        std::pair<unsigned int, STGuildWarInfo*> p = *it;
        if (p.second != 0)
        {
            rank++;
            DNF_LOG_SCOPE_LINE(0x192,"./log/GuildWar",
                "GuildKey : %d(%s),  GuildWarPoint : %d, GuildWarRank : %d",
                p.second->m_guildKey, p.second->m_data,
                p.second->m_point, rank);
        }
    }
}

Packet_Notice_DB_Save_Guild_War_Point::Packet_Notice_DB_Save_Guild_War_Point()
    : PacketHeader(0x418, 0x5b)
{
    memset((char*)this + 0xb, 0, 0x28);
    memset((char*)this + 0x33, 0, 0x28);
}

template void DNFFLib::Swap<STGuildWarInfo>(STGuildWarInfo*, STGuildWarInfo*);
