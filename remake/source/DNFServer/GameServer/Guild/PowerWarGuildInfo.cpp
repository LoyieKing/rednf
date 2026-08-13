// df_guild_r — PowerWarGuildInfo (split from source/guild per ORIG layout)
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
#include <stdio.h>
#include <utility>
#include <stdlib.h>
#include <sys/time.h>

#include "PowerWarGuildInfo.h"
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
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

MemPool<STPowerWarGuildInfo> m_PowerWarGuildInfoMemPool_(1000);

void* STPowerWarGuildInfo::operator new(unsigned int size) { return m_PowerWarGuildInfoMemPool_.alloc(); }

void STPowerWarGuildInfo::operator delete(void* p) { m_PowerWarGuildInfoMemPool_.free(p); }

void STPowerWarGuildInfo::operator delete(void* p, unsigned int size) { m_PowerWarGuildInfoMemPool_.free(p, size); }

CPowerWarGuildInfo::CPowerWarGuildInfo()
{
}

CPowerWarGuildInfo::~CPowerWarGuildInfo()
{
}

void CPowerWarGuildInfo::Initialize()
{
    m_map.clear();
    m_vec.clear();
    m_vec2.clear();
}

void CPowerWarGuildInfo::Clean()
{
    for (std::vector<STPowerWarGuildInfo*>::iterator it = m_vec.begin();
         it != m_vec.end(); ++it)
    {
        delete *it;
    }
    m_vec.clear();
    m_map.clear();
}

STPowerWarGuildInfo* CPowerWarGuildInfo::CreatePowerwarGuild()
{
    STPowerWarGuildInfo* info = 0;
    info = new STPowerWarGuildInfo;
    memset(info, 0, 4);
    return info;
}

void CPowerWarGuildInfo::DeletePowerWarGuild(STPowerWarGuildInfo* info)
{
    if (info != 0)
    {
        delete info;
    }
}

STPowerWarGuildInfo* CPowerWarGuildInfo::FindPowerwarGuild(unsigned int guildKey)
{
    std::map<unsigned int, STPowerWarGuildInfo*>::iterator it = m_map.find(guildKey);
    if (it != m_map.end())
    {
        return it->second;
    }
    return 0;
}

int CPowerWarGuildInfo::InsertPowerwarGuild(unsigned int guildKey, STPowerWarGuildInfo* info)
{
    m_map.insert(std::make_pair(guildKey, info));
    m_vec.push_back(info);
}

void CPowerWarGuildInfo::UpdateGuildPowerwarInfo(unsigned int guildKey, unsigned short point)
{
    STPowerWarGuildInfo* info = FindPowerwarGuild(guildKey);
    if (info != 0)
    {
        info->m_field[1] = info->m_field[1] + point;
    }
    else
    {
        info = CreatePowerwarGuild();
        info->m_field[0] = guildKey;
        info->m_field[1] = point;
        InsertPowerwarGuild(guildKey, info);
    }
}

void CPowerWarGuildInfo::CalcAllGuildRanking()
{
    std::sort(m_vec.begin(), m_vec.end(), STPowerWarGuildInfo::Compare);
}

unsigned int CPowerWarGuildInfo::GetGuildRanking(unsigned int guildKey)
{
    std::vector<STPowerWarGuildInfo*>::iterator it = m_vec.begin();
    unsigned int rank = 1;
    for (; it != m_vec.end(); ++it)
    {
        STPowerWarGuildInfo* info = *it;
        if (info->m_field[0] == guildKey)
        {
            return rank;
        }
        rank++;
    }
    return 0;
}

STPowerWarGuildInfo* CPowerWarGuildInfo::GetSpecificGuildInfo(unsigned int guildKey)
{
    std::map<unsigned int, STPowerWarGuildInfo*>::iterator it = m_map.find(guildKey);
    if (it != m_map.end())
    {
        return it->second;
    }
    return 0;
}

void CPowerWarGuildInfo::GetAllGuildRankingInfo(int& count, STGuildRank* rank)
{
    unsigned int n = 0;
    for (std::vector<STPowerWarGuildInfo*>::iterator it = m_vec.begin();
         it != m_vec.end() && n < 100; ++it)
    {
        STPowerWarGuildInfo* info = *it;
        if (info != 0)
        {
            rank[n].m0 = info->m_field[0];
            rank[n].m4 = info->m_field[3];
            n++;
        }
    }
    count = (int)n;
}

void CPowerWarGuildInfo::RewardGuildPowerWarPoint(CGuildManager& gm, bool a, int b, int c,
                                                 int d, int e)
{
    int basic = b;
    int firstBonus = c;
    int decrease = d;
    int maxGrade = e;
    int rankIdx = 0;
    unsigned int point = 0;
    unsigned int count = (unsigned int)m_vec.size();
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0xec,"./log/PowerResult",
        "Basic:%d, First:%d, Decrease:%d, MaxGrade:%d, DomainCount:%d", basic, firstBonus,
        decrease, maxGrade, count);
    for (std::vector<STPowerWarGuildInfo*>::iterator it = m_vec.begin(); it != m_vec.end(); ++it)
    {
        STPowerWarGuildInfo* info = *it;
        point = 0;
        if (a)
        {
            point = basic;
        }
        point += info->m_field[1];
        if (info->m_field[1] != 0 && rankIdx < maxGrade)
        {
            int v = firstBonus - decrease * rankIdx;
            info->m_field[2] = (unsigned int)(~(int)(v >> 31) & v);
            point += info->m_field[2];
            info->m_field[3] = point;
        }
        unsigned int guildKey = info->m_field[0];
        CGuild* guild = gm.FindGuild(guildKey);
        if (guild == 0)
        {
            STDBSavePowerWarPoint* p = CreateDBSavePowerWarPoint();
            if (p != 0)
            {
                p->m_field[0] = guildKey;
                p->m_field[1] = info->m_field[3];
                m_vec2.push_back(p);
                DNF_LOG_SCOPE_AT(__FUNCTION__, 0x120,"./log/PowerResult",
                    "Additional Save(GRADE:%d, Guild ID:%d, PowerWarPoint:%d)", rankIdx,
                    p->m_field[0], p->m_field[1]);
            }
        }
        else
        {
            guild->AddPowerWarPoint(point);
            guild->SendGuildInfoToMembers(false);
        }
        rankIdx++;
    }
}

STDBSavePowerWarPoint* CPowerWarGuildInfo::CreateDBSavePowerWarPoint()
{
    STDBSavePowerWarPoint* p = 0;
    p = new (std::nothrow) STDBSavePowerWarPoint();
    if (p != 0)
    {
        memset(p, 0, 4);
    }
    return p;
}

void CPowerWarGuildInfo::DeleteDBSavePowerWarPoint(STDBSavePowerWarPoint* p)
{
    if (p != 0)
    {
        delete p;
        p = 0;
    }
}

void CPowerWarGuildInfo::MakePacketDBPowerWarPoint(Packet_DB_Save_Power_War_Point_Reward* pkt)
{
    size_t n = m_vec2.size();
    int count = 0;
    int i = 0;
    if (n != 0)
    {
        if (n <= 0xfa)
        {
            count = (int)n;
        }
        else
        {
            count = 0xfa;
        }
        char* out = (char*)pkt + 0xf;
        for (std::vector<STDBSavePowerWarPoint*>::iterator it = m_vec2.begin();
             it != m_vec2.end() && i <= count; )
        {
            STDBSavePowerWarPoint* p = *it;
            *(unsigned int*)(out + i * 8) = p->m_field[0];
            *(unsigned int*)(out + i * 8 + 4) = p->m_field[1];
            DNF_LOG_SCOPE_LINE(0x16b,"./log/Power", "INTERVAL SAVE - GUILD:%d, POINT:%d",
                p->m_field[0], p->m_field[1]);
            DeleteDBSavePowerWarPoint(p);
            it = m_vec2.erase(it);
            i++;
        }
        pkt->m_b = (unsigned int)count;
    }
}

int CPowerWarGuildInfo::GetPowerWarPointDBSaveCount()
{
    return (int)m_vec2.size();
}

void CPowerWarGuildInfo::PrintDebugInfo()
{
    CMyFileLog log1(__FUNCTION__, 0x187);
    log1("./log/Power",
         "------ POWER WAR GUILD DEBUG INFO START ----------------------------------------------------------");
    CMyFileLog log2(__FUNCTION__, 0x188);
    log2("./log/Power",
         "------ ALL GUILD RANKING -------------------------------------------------------------------------");
    std::vector<STPowerWarGuildInfo*>::iterator it = m_vec.begin();
    int rank = 1;
    for (; it != m_vec.end(); ++it)
    {
        STPowerWarGuildInfo* info = *it;
        DNF_LOG_SCOPE_LINE(0x192,"./log/Power", "RANK:%d, GUILD:%d, POINT:%d, TOTOAL:%d, BONUS:%d", rank,
            info->m_field[0], info->m_field[1],
            info->m_field[3], info->m_field[2]);
        rank++;
    }
    CMyFileLog log3(__FUNCTION__, 0x199);
    log3("./log/Power",
         "------ POWER WAR GUILD DEBUG INFO END   ----------------------------------------------------------");
}

STPowerWarGuildInfo::STPowerWarGuildInfo()
{
    m_field[0] = 0;
    m_field[1] = 0;
    m_field[2] = 0;
    m_field[3] = 0;
}

bool STPowerWarGuildInfo::Compare(const STPowerWarGuildInfo* a, const STPowerWarGuildInfo* b)
{
    if (a->m_field[1] > b->m_field[1])
        return true;
    return false;
}

STDBSavePowerWarPoint::STDBSavePowerWarPoint()
{
    m_field[0] = 0;
    m_field[1] = 0;
}

template std::allocator<std::_List_node<STUserPoint> >::allocator();
template std::allocator<std::_List_node<STUserPoint> >::~allocator();
