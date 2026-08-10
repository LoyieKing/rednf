// df_guild_r — PowerWarCharacInfo (split from source/guild per ORIG layout)
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

#include "PowerWarCharacInfo.h"
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
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

MemPool<STPowerWarCharacInfo> m_PowerWarCharacInfoMemPool_(1000);

void* STPowerWarCharacInfo::operator new(unsigned int size) { return m_PowerWarCharacInfoMemPool_.alloc(); }

void STPowerWarCharacInfo::operator delete(void* p) { m_PowerWarCharacInfoMemPool_.free(p); }

void STPowerWarCharacInfo::operator delete(void* p, unsigned int size) { m_PowerWarCharacInfoMemPool_.free(p, size); }

bool STPowerWarCharacInfo::Compare(const STPowerWarCharacInfo* a, const STPowerWarCharacInfo* b)
{
    return *(unsigned int*)(b->m_data + 4) < *(unsigned int*)(a->m_data + 4);
}

std::vector<STPowerWarCharacInfo*>* CPowerWarCharacInfo::GetCharacInfoVector()
{
    return (std::vector<STPowerWarCharacInfo*>*)(m_data + 0x18);
}

CPowerWarCharacInfo::CPowerWarCharacInfo()
{
    new (m_data + 0) std::map<unsigned int, STPowerWarCharacInfo*>();       // class +4
    new (m_data + 0x18) std::vector<STPowerWarCharacInfo*>();               // class +0x1c
}

CPowerWarCharacInfo::~CPowerWarCharacInfo()
{
    ((std::vector<STPowerWarCharacInfo*>*)(m_data + 0x18))->~vector();
    ((std::map<unsigned int, STPowerWarCharacInfo*>*)(m_data + 0))->~map();
}

void CPowerWarCharacInfo::Initialize()
{
    ((std::map<unsigned int, STPowerWarCharacInfo*>*)m_data)->clear();
    ((std::vector<STPowerWarCharacInfo*>*)(m_data + 0x18))->clear();
    ((std::list<STUserPoint>*)(m_data + 0x24))->clear();
}

void CPowerWarCharacInfo::Clean()
{
    std::vector<STPowerWarCharacInfo*>* chars =
        (std::vector<STPowerWarCharacInfo*>*)((char*)this + 0x1c);
    for (std::vector<STPowerWarCharacInfo*>::iterator it = chars->begin();
         it != chars->end(); ++it)
    {
        delete *it;
    }
    chars->clear();
    ((std::map<unsigned int, STPowerWarCharacInfo*>*)m_data)->clear();
}

STPowerWarCharacInfo* CPowerWarCharacInfo::CreatePowerwarCharac()
{
    STPowerWarCharacInfo* info = new STPowerWarCharacInfo;
    memset(info, 0, 4);
    return info;
}

STPowerWarCharacInfo* CPowerWarCharacInfo::FindPowerwarCharac(unsigned int charNo)
{
    std::map<unsigned int, STPowerWarCharacInfo*>* map =
        (std::map<unsigned int, STPowerWarCharacInfo*>*)(m_data + 0);
    std::map<unsigned int, STPowerWarCharacInfo*>::iterator it = map->find(charNo);
    if (it == map->end())
    {
        return 0;
    }
    return it->second;
}

int CPowerWarCharacInfo::InsertPowerwarCharac(unsigned int charNo, STPowerWarCharacInfo* info)
{
    std::map<unsigned int, STPowerWarCharacInfo*>* map =
        (std::map<unsigned int, STPowerWarCharacInfo*>*)(m_data + 0);
    map->insert(std::make_pair(charNo, info));
    ((std::vector<STPowerWarCharacInfo*>*)(m_data + 0x18))->push_back(info);
    return 0;
}

void CPowerWarCharacInfo::UpdatePowerwarCharacInfo(unsigned int charNo, unsigned short point)
{
    STPowerWarCharacInfo* info = FindPowerwarCharac(charNo);
    if (info == 0)
    {
        info = CreatePowerwarCharac();
        *(unsigned int*)info->m_data = charNo;
        *(unsigned int*)(info->m_data + 4) = point;
        InsertPowerwarCharac(charNo, info);
    }
    else
    {
        *(unsigned int*)(info->m_data + 4) =
            *(unsigned int*)(info->m_data + 4) + point;
    }
}

void CPowerWarCharacInfo::CalcAllUserRanking()
{
    std::vector<STPowerWarCharacInfo*>* vec =
        (std::vector<STPowerWarCharacInfo*>*)(m_data + 0x18);
    std::sort(vec->begin(), vec->end(), STPowerWarCharacInfo::Compare);
}

unsigned int CPowerWarCharacInfo::GetUserRanking(unsigned int charNo)
{
    std::vector<STPowerWarCharacInfo*>* vec =
        (std::vector<STPowerWarCharacInfo*>*)(m_data + 0x18);
    unsigned int rank = 1;
    for (std::vector<STPowerWarCharacInfo*>::iterator it = vec->begin(); it != vec->end(); ++it)
    {
        if (*(unsigned int*)(*it)->m_data == charNo)
        {
            return rank;
        }
        rank++;
    }
    return 0;
}

unsigned int CPowerWarCharacInfo::GetUserPowerWarPoint(unsigned int charNo)
{
    std::map<unsigned int, STPowerWarCharacInfo*>* map =
        (std::map<unsigned int, STPowerWarCharacInfo*>*)(m_data + 0);
    std::map<unsigned int, STPowerWarCharacInfo*>::iterator it = map->find(charNo);
    if (it == map->end())
    {
        return 0;
    }
    return *(unsigned int*)(it->second->m_data + 4);
}

void CPowerWarCharacInfo::GetAllUserRankingInfo(unsigned int& count, STUserRank* rank)
{
    unsigned int n = 0;
    std::vector<STPowerWarCharacInfo*>* vec =
        (std::vector<STPowerWarCharacInfo*>*)(m_data + 0x18);
    for (std::vector<STPowerWarCharacInfo*>::iterator it = vec->begin();
         it != vec->end() && n < 500; ++it)
    {
        STPowerWarCharacInfo* info = *it;
        if (info != 0)
        {
            unsigned int charNo = *(unsigned int*)info->m_data;
            unsigned int point = *(unsigned int*)(info->m_data + 4);
            *(unsigned int*)(rank + n * 8) = charNo;
            *(unsigned int*)(rank + n * 8 + 4) = point;
            n++;
            DNF_LOG_SCOPE_LINE(0xcc, "./log/Power", "Rank:%7d, Charac No:%d, PowerWarPoint:%d", n, charNo, point);
        }
    }
    count = n;
}

void CPowerWarCharacInfo::GetStatueRankingUsers(std::vector<STPowerWarCharacInfo*>& vec)
{
    std::vector<STPowerWarCharacInfo*>* self = GetCharacInfoVector();
    for (unsigned int i = 0; i < 3; i++)
    {
        if ((int)i < (int)self->size())
        {
            STPowerWarCharacInfo* info = self->at(i);
            vec.push_back(info);
            DNF_LOG_SCOPE_LINE(0xe4,"./log/Power", "Rank:%d, Charac No:%d Point:%d", i,
                *(unsigned int*)info->m_data, *(unsigned int*)(info->m_data + 4));
        }
    }
}

void CPowerWarCharacInfo::PrintDebugInfo()
{
    CMyFileLog log1("PrintDebugInfo", 0xee);
    log1("./log/PowerResult",
         "------ POWER WAR CHARAC DEBUG INFO START --------------------------------------------------------");
    CMyFileLog log2("PrintDebugInfo", 0xef);
    log2("./log/PowerResult",
         "------ ALL USER RANKING -------------------------------------------------------------------------");
    int rank = 1;
    std::vector<STPowerWarCharacInfo*>* vec =
        (std::vector<STPowerWarCharacInfo*>*)(m_data + 0x18);
    for (std::vector<STPowerWarCharacInfo*>::iterator it = vec->begin(); it != vec->end(); ++it)
    {
        STPowerWarCharacInfo* info = *it;
        DNF_LOG_SCOPE_LINE(0xf9,"./log/PowerResult", "RANK:%d, USER:%d, POWER WAR POINT:%d", rank,
            *(unsigned int*)info->m_data, *(unsigned int*)(info->m_data + 4));
        rank++;
    }
    CMyFileLog log3("PrintDebugInfo", 0x100);
    log3("./log/PowerResult",
         "------ POWER WAR CHARAC DEBUG INFO END   --------------------------------------------------------");
}

int CPowerWarCharacInfo::IsExistCharac(unsigned int charNo)
{
    std::map<unsigned int, STPowerWarCharacInfo*>* map =
        (std::map<unsigned int, STPowerWarCharacInfo*>*)(m_data + 0);
    std::map<unsigned int, STPowerWarCharacInfo*>::iterator it = map->find(charNo);
    return it != map->end();
}

void CPowerWarCharacInfo::CalcBonus()
{
    std::vector<STPowerWarCharacInfo*>* chars =
        (std::vector<STPowerWarCharacInfo*>*)((char*)this + 0x1c);
    std::list<STUserPoint>* points =
        (std::list<STUserPoint>*)((char*)this + 0x28);
    for (std::vector<STPowerWarCharacInfo*>::iterator it = chars->begin();
         it != chars->end(); ++it)
    {
        STUserPoint pt;
        *(unsigned int*)((char*)&pt + 0) = *(unsigned int*)(*it)->m_data;
        *(unsigned int*)((char*)&pt + 4) =
            (unsigned int)GetBonus(*(int*)((*it)->m_data + 4));
        points->push_back(pt);
    }
}

int CPowerWarCharacInfo::GetBonus(Packet_DB_Save_Power_War_Bonus_Point& pkt)
{
    std::list<STUserPoint>* points =
        (std::list<STUserPoint>*)((char*)this + 0x28);
    int i = 0;
    std::list<STUserPoint>::iterator it = points->begin();
    while (it != points->end() && i <= 0xf9)
    {
        *(unsigned int*)((char*)&pkt + 0xe + i * 8) = *(unsigned int*)((char*)&(*it) + 0);
        *(unsigned int*)((char*)&pkt + 0x12 + i * 8) = *(unsigned int*)((char*)&(*it) + 4);
        i++;
        it = points->erase(it);
    }
    *(unsigned int*)((char*)&pkt + 0xa) = (unsigned int)i;
    return 0;
}

int CPowerWarCharacInfo::GetBonus(int idx)
{
    if (idx < 0x1f)
    {
        return 1;
    }
    if (idx < 0x3d)
    {
        return 2;
    }
    if (idx < 0x65)
    {
        return 3;
    }
    if (idx < 0x97)
    {
        return 4;
    }
    if (idx < 0xc9)
    {
        return 5;
    }
    return 6;
}

STPowerWarCharacInfo::STPowerWarCharacInfo()
{
    memset(m_data, 0, sizeof(m_data));
}

