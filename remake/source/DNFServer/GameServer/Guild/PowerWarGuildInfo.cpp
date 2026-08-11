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
    std::vector<STPowerWarGuildInfo*>* guilds = &m_vec;
    for (std::vector<STPowerWarGuildInfo*>::iterator it = guilds->begin();
         it != guilds->end(); ++it)
    {
        delete *it;
    }
    guilds->clear();
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
    std::map<unsigned int, STPowerWarGuildInfo*>* map = &m_map;
    std::map<unsigned int, STPowerWarGuildInfo*>::iterator it = map->find(guildKey);
    if (it == map->end())
    {
        return 0;
    }
    return it->second;
}

int CPowerWarGuildInfo::InsertPowerwarGuild(unsigned int guildKey, STPowerWarGuildInfo* info)
{
    std::map<unsigned int, STPowerWarGuildInfo*>* map = &m_map;
    map->insert(std::make_pair(guildKey, info));
    m_vec.push_back(info);
    return 0;
}

void CPowerWarGuildInfo::UpdateGuildPowerwarInfo(unsigned int guildKey, unsigned short point)
{
    STPowerWarGuildInfo* info = FindPowerwarGuild(guildKey);
    if (info == 0)
    {
        info = CreatePowerwarGuild();
        *(unsigned int*)info->m_data = guildKey;
        *(unsigned int*)(info->m_data + 4) = point;
        InsertPowerwarGuild(guildKey, info);
    }
    else
    {
        *(unsigned int*)(info->m_data + 4) =
            *(unsigned int*)(info->m_data + 4) + point;
    }
}

void CPowerWarGuildInfo::CalcAllGuildRanking()
{
    std::vector<STPowerWarGuildInfo*>* vec = &m_vec;
    std::sort(vec->begin(), vec->end(), STPowerWarGuildInfo::Compare);
}

unsigned int CPowerWarGuildInfo::GetGuildRanking(unsigned int guildKey)
{
    std::vector<STPowerWarGuildInfo*>* vec = &m_vec;
    unsigned int rank = 1;
    for (std::vector<STPowerWarGuildInfo*>::iterator it = vec->begin(); it != vec->end(); ++it)
    {
        if (*(unsigned int*)(*it)->m_data == guildKey)
        {
            return rank;
        }
        rank++;
    }
    return 0;
}

STPowerWarGuildInfo* CPowerWarGuildInfo::GetSpecificGuildInfo(unsigned int guildKey)
{
    std::map<unsigned int, STPowerWarGuildInfo*>* map = &m_map;
    std::map<unsigned int, STPowerWarGuildInfo*>::iterator it = map->find(guildKey);
    if (it == map->end())
    {
        return 0;
    }
    return it->second;
}

void CPowerWarGuildInfo::GetAllGuildRankingInfo(int& count, STGuildRank* rank)
{
    unsigned int n = 0;
    std::vector<STPowerWarGuildInfo*>* vec = &m_vec;
    for (std::vector<STPowerWarGuildInfo*>::iterator it = vec->begin();
         it != vec->end() && n < 100; ++it)
    {
        STPowerWarGuildInfo* info = *it;
        if (info != 0)
        {
            *(unsigned int*)(rank + n * 8) = *(unsigned int*)info->m_data;
            *(unsigned int*)(rank + n * 8 + 4) = *(unsigned int*)(info->m_data + 0xc);
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
    std::vector<STPowerWarGuildInfo*>* vec = &m_vec;
    unsigned int count = (unsigned int)vec->size();
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0xec,"./log/PowerResult",
        "Basic:%d, First:%d, Decrease:%d, MaxGrade:%d, DomainCount:%d", basic, firstBonus,
        decrease, maxGrade, count);
    for (std::vector<STPowerWarGuildInfo*>::iterator it = vec->begin(); it != vec->end(); ++it)
    {
        STPowerWarGuildInfo* info = *it;
        unsigned int point = 0;
        if (a)
        {
            point = (unsigned int)basic;
        }
        point += *(unsigned int*)(info->m_data + 4);
        if (*(unsigned int*)(info->m_data + 4) != 0 && rankIdx < maxGrade)
        {
            unsigned int v = (unsigned int)(firstBonus - decrease * rankIdx);
            *(unsigned int*)(info->m_data + 8) = (unsigned int)(~(int)(v >> 31) & v);
            point += *(unsigned int*)(info->m_data + 8);
            *(unsigned int*)(info->m_data + 0xc) = point;
        }
        unsigned int guildKey = *(unsigned int*)info->m_data;
        CGuild* guild = gm.FindGuild(guildKey);
        if (guild == 0)
        {
            STDBSavePowerWarPoint* p = CreateDBSavePowerWarPoint();
            if (p != 0)
            {
                *(unsigned int*)p = guildKey;
                *(unsigned int*)((char*)p + 4) = *(unsigned int*)(info->m_data + 0xc);
                m_vec2.push_back(p);
                DNF_LOG_SCOPE_AT("RewardGuildPowerWarPoint", 0x120,"./log/PowerResult",
                    "Additional Save(GRADE:%d, Guild ID:%d, PowerWarPoint:%d)", rankIdx,
                    guildKey, *(unsigned int*)((char*)p + 4));
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
    STDBSavePowerWarPoint* p = new (std::nothrow) STDBSavePowerWarPoint();
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
    std::vector<STDBSavePowerWarPoint*>* vec = &m_vec2;
    size_t n = vec->size();
    int count = 0;
    if (n != 0)
    {
        count = n < 0xfb ? (int)n : 0xfa;
        char* out = (char*)pkt + 0xf;
        int i = 0;
        for (std::vector<STDBSavePowerWarPoint*>::iterator it = vec->begin();
             it != vec->end() && i < count; )
        {
            STDBSavePowerWarPoint* p = *it;
            *(unsigned int*)(out + i * 8) = *(unsigned int*)p;
            *(unsigned int*)(out + i * 8 + 4) = *(unsigned int*)((char*)p + 4);
            DNF_LOG_SCOPE_LINE(0x16b,"./log/Power", "INTERVAL SAVE - GUILD:%d, POINT:%d",
                *(unsigned int*)p, *(unsigned int*)((char*)p + 4));
            DeleteDBSavePowerWarPoint(p);
            it = vec->erase(it);
            i++;
        }
        *(unsigned int*)((char*)pkt + 0xb) = (unsigned int)count;
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
    CMyFileLog log2("PrintDebugInfo", 0x188);
    log2("./log/Power",
         "------ ALL GUILD RANKING -------------------------------------------------------------------------");
    int rank = 1;
    std::vector<STPowerWarGuildInfo*>* vec = &m_vec;
    for (std::vector<STPowerWarGuildInfo*>::iterator it = vec->begin(); it != vec->end(); ++it)
    {
        STPowerWarGuildInfo* info = *it;
        DNF_LOG_SCOPE_LINE(0x192,"./log/Power", "RANK:%d, GUILD:%d, POINT:%d, TOTOAL:%d, BONUS:%d", rank,
            *(unsigned int*)info->m_data, *(unsigned int*)(info->m_data + 4),
            *(unsigned int*)(info->m_data + 0xc), *(unsigned int*)(info->m_data + 8));
        rank++;
    }
    CMyFileLog log3(__FUNCTION__, 0x199);
    log3("./log/Power",
         "------ POWER WAR GUILD DEBUG INFO END   ----------------------------------------------------------");
}

STPowerWarGuildInfo::STPowerWarGuildInfo()
{
    memset(m_data, 0, sizeof(m_data));
}

bool STPowerWarGuildInfo::Compare(const STPowerWarGuildInfo* a, const STPowerWarGuildInfo* b)
{
    return *(unsigned int*)(b->m_data + 4) < *(unsigned int*)(a->m_data + 4);
}

STDBSavePowerWarPoint::STDBSavePowerWarPoint()
{
    memset(m_data, 0, sizeof(m_data));
}

template std::allocator<std::_List_node<STUserPoint> >::allocator();
template std::allocator<std::_List_node<STUserPoint> >::~allocator();
