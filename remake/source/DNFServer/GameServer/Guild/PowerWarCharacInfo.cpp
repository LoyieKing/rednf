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
    if (a->m_field[1] > b->m_field[1])
        return true;
    return false;
}

std::vector<STPowerWarCharacInfo*>* CPowerWarCharacInfo::GetCharacInfoVector()
{
    return &m_vec;
}

CPowerWarCharacInfo::CPowerWarCharacInfo()
{
}

CPowerWarCharacInfo::~CPowerWarCharacInfo()
{
}

void CPowerWarCharacInfo::Initialize()
{
    m_map.clear();
    m_vec.clear();
    m_list.clear();
}

void CPowerWarCharacInfo::Clean()
{
    for (std::vector<STPowerWarCharacInfo*>::iterator it = m_vec.begin();
         it != m_vec.end(); ++it)
    {
        delete *it;
    }
    m_vec.clear();
    m_map.clear();
}

STPowerWarCharacInfo* CPowerWarCharacInfo::CreatePowerwarCharac()
{
    STPowerWarCharacInfo* info = 0;
    info = new STPowerWarCharacInfo;
    memset(info, 0, 4);
    return info;
}

STPowerWarCharacInfo* CPowerWarCharacInfo::FindPowerwarCharac(unsigned int charNo)
{
    std::map<unsigned int, STPowerWarCharacInfo*>::iterator it = m_map.find(charNo);
    if (it != m_map.end())
    {
        return it->second;
    }
    return 0;
}

int CPowerWarCharacInfo::InsertPowerwarCharac(unsigned int charNo, STPowerWarCharacInfo* info)
{
    m_map.insert(std::make_pair(charNo, info));
    m_vec.push_back(info);
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}

void CPowerWarCharacInfo::UpdatePowerwarCharacInfo(unsigned int charNo, unsigned short point)
{
    STPowerWarCharacInfo* info = FindPowerwarCharac(charNo);
    if (info != 0)
    {
        info->m_field[1] = info->m_field[1] + point;
    }
    else
    {
        info = CreatePowerwarCharac();
        info->m_field[0] = charNo;
        info->m_field[1] = point;
        InsertPowerwarCharac(charNo, info);
    }
}

void CPowerWarCharacInfo::CalcAllUserRanking()
{
    std::sort(m_vec.begin(), m_vec.end(), STPowerWarCharacInfo::Compare);
}

unsigned int CPowerWarCharacInfo::GetUserRanking(unsigned int charNo)
{
    std::vector<STPowerWarCharacInfo*>::iterator it = m_vec.begin();
    unsigned int rank = 1;
    for (; it != m_vec.end(); ++it)
    {
        STPowerWarCharacInfo* info = *it;
        if (info->m_field[0] == charNo)
        {
            return rank;
        }
        rank++;
    }
    return 0;
}

unsigned int CPowerWarCharacInfo::GetUserPowerWarPoint(unsigned int charNo)
{
    std::map<unsigned int, STPowerWarCharacInfo*>::iterator it = m_map.find(charNo);
    if (it != m_map.end())
    {
        return it->second->m_field[1];
    }
    return 0;
}

void CPowerWarCharacInfo::GetAllUserRankingInfo(unsigned int& count, STUserRank* rank)
{
    unsigned int n = 0;
    for (std::vector<STPowerWarCharacInfo*>::iterator it = m_vec.begin();
         it != m_vec.end() && n < 500; ++it)
    {
        STPowerWarCharacInfo* info = *it;
        if (info != 0)
        {
            rank[n].m0 = info->m_field[0];
            rank[n].m4 = info->m_field[1];
            n++;
            DNF_LOG_SCOPE_LINE(0xcc, "./log/Power", "Rank:%7d, Charac No:%d, PowerWarPoint:%d", n,
                info->m_field[0], info->m_field[1]);
        }
    }
    count = n;
}

void CPowerWarCharacInfo::GetStatueRankingUsers(std::vector<STPowerWarCharacInfo*>& vec)
{
    // [P183/P339] ORIG 的 i 为 **signed int** 而循环界为 unsigned（`cmp $0x2; setbe`
    //   = `i < 3u`），比较式 `(int)size() > i` 才是单条 `cmp mem,reg; setg`；
    //   三处都是即时 `at(i)`（无具名 info 缓存），字段用 `m_field[]` disp 折叠。
    for (int i = 0; i < 3u; i++)
    {
        if ((int)m_vec.size() > i)
        {
            vec.push_back(m_vec.at(i));
            DNF_LOG_SCOPE_LINE(0xe4,"./log/Power", "Rank:%d, Charac No:%d Point:%d", i,
                m_vec.at(i)->m_field[0], m_vec.at(i)->m_field[1]);
        }
    }
}

void CPowerWarCharacInfo::PrintDebugInfo()
{
    DNF_LOG_SCOPE_LINE(0xee, "./log/PowerResult",
        "------ POWER WAR CHARAC DEBUG INFO START --------------------------------------------------------");
    DNF_LOG_SCOPE_LINE(0xef, "./log/PowerResult",
        "------ ALL USER RANKING -------------------------------------------------------------------------");
    std::vector<STPowerWarCharacInfo*>::iterator it = m_vec.begin();
    int rank = 1;
    for (; it != m_vec.end(); ++it)
    {
        STPowerWarCharacInfo* info = *it;
        DNF_LOG_SCOPE_LINE(0xf9,"./log/PowerResult", "RANK:%d, USER:%d, POWER WAR POINT:%d", rank,
            info->m_field[0], info->m_field[1]);
        rank++;
    }
    DNF_LOG_SCOPE_LINE(0x100, "./log/PowerResult",
        "------ POWER WAR CHARAC DEBUG INFO END   --------------------------------------------------------");
}

int CPowerWarCharacInfo::IsExistCharac(unsigned int charNo)
{
    std::map<unsigned int, STPowerWarCharacInfo*>::iterator it = m_map.find(charNo);
    if (it != m_map.end())
    {
        return 1;
    }
    return 0;
}

void CPowerWarCharacInfo::CalcBonus()
{
    std::vector<STPowerWarCharacInfo*>::iterator it = m_vec.begin();
    for (; it != m_vec.end(); ++it)
    {
        STUserPoint pt;
        pt.m0 = (*it)->m_field[0];
        pt.m4 = (unsigned int)GetBonus((int)(*it)->m_field[1]);
        m_list.push_back(pt);
    }
}

void CPowerWarCharacInfo::GetBonus(Packet_DB_Save_Power_War_Bonus_Point& pkt)
{
    std::list<STUserPoint>::iterator it = m_list.begin();
    int i = 0;
    for (; it != m_list.end() && i <= 0xf9U; )
    {
        pkt.m_points[i].m0 = (*it).m0;
        pkt.m_points[i].m4 = (*it).m4;
        i++;
        it = m_list.erase(it);
    }
    pkt.m_bonus = i;
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
    m_field[0] = 0;
    m_field[1] = 0;
}
