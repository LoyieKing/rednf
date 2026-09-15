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

#include "../Game/ST_Guild_War_Rank_Info.h"
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
            c[idx] = (unsigned short)++idx;  // ORIG: addr 用旧 idx，值 = 新 idx（++ 先落槽）
        }
    }
}

// [DNF-IDENTICAL（2026-09-15 复核：已 identical ✓）] DNF-GLD-DIFF-0149 | guild | 与ORIG差异=DIFF | CGuildWar::GetGuildWarInfo | 详见 function_reports/guild/_ZN9CGuildWar15GetGuildWarInfoEP22ST_Guild_War_Rank_Info.md
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
            info[count].m_field0 = it->second->m_guildKey;
            info[count].m_field4 = it->second->m_point;
            memcpy(info[count].m_pad0a, it->second->m_data, 0x16);
            info[count].m_field8 = (short)++count;
        }
    }
}

void CGuildWar::Insert_GuildWarInfo(STGuildWarInfo* info)
{
    if (info != 0)
    {
        DNF_LOG_SCOPE_LINE(0x90,"./log/GuildWar", "[INSERT]\tGuild Key : %d\tGuild Point : %d\n",
            info->m_guildKey, info->m_point);
        m_vtGuildWarInfo.push_back(std::make_pair(info->m_point, info));
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x95, "./log/GuildWar", "[INSERT_ERR]info == 0\n");
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
    // [2026-09-15 修复] ORIG 0x8066b34 先载 0x8(%ebp)（a）再载 0xc(%ebp)（b），
    //   尾为 `cmp %eax,%edx; seta %al` ⇒ 原码是 `a > b`（写成 `b < a` 会反出
    //   setb + 装载序颠倒；两者语义同为降序比较器）。
    return a.second->m_point > b.second->m_point;
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

// [DNF-IDENTICAL（2026-09-15 复核：已 identical ✓）] DNF-GLD-DIFF-0147 | guild | 与ORIG差异=DIFF | CGuildWar::SameRankWork | 详见 function_reports/guild/_ZN9CGuildWar12SameRankWorkEv.md
bool CGuildWar::SameRankWork()
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
        // [2026-09-15 GuildA] ORIG 在 `maxIt = it2` 处经引用读 it2（lea+mov (%eax)）：
        //   原码是**链式赋值** `maxIt = it2 = begin()`（GCC O0 对内层赋值结果经地址取）。
        //   实测 ndiff 2→0。
        maxIt = it2 = m_vtGuildWarInfo.begin();
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
        // [2026-09-15 GuildA] X3 判据：ORIG 此处调 `_Rb_tree_iterator::operator*`（de），
        //   写成 `maxIt->second` 会调 `operator->`（ptEv）⇒ 剥目标口径（.o, mdiff）看不到
        //   该差异，服务级 `compare_svc_full`/`mdiff_ta` 判为 NEAR。改 `(*maxIt).second` 后对齐。
        if ((*maxIt).second->m_field[6] != maxVal)
        {
            DNFFLib::Swap<STGuildWarInfo>((*maxIt).second, (*maxItTmp).second);
        }
    }
    return 1;
}

bool CGuildWar::IsGuildWarEnterableGuild(unsigned int guildId)
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
    while ((unsigned char)m_bRankCnt <= 1)
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

// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0148 | guild | 与ORIG差异=DIFF | CGuildWar::DBSaveProcess | 详见 function_reports/guild/_ZN9CGuildWar13DBSaveProcessEP12CApplication.md
void CGuildWar::DBSaveProcess(CApplication* app)
{
    if (!IsGuildWarEventOn())
    {
        return;
    }
    m_bSaveCnt++;
    // [机理 B] ORIG 无 push %ebx 且 `sete %al`（帧 `sub $0x78`）⇒ 用 asm 绑定固定到 al。
    register bool bZero __asm__("al") = (m_bSaveCnt == 0);
    if (bZero)
    {
        return;
    }
    Packet_Notice_DB_Save_Guild_War_Point pkt;
    if (GetGuildWarInfoDBSave(pkt.m_guildKeys, pkt.m_points))
    {
        pkt.m_group = app->Get_ServerGroup();
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
    memset(m_guildKeys, 0, sizeof(m_guildKeys));
    memset(m_points, 0, sizeof(m_points));
}

template void DNFFLib::Swap<STGuildWarInfo>(STGuildWarInfo*, STGuildWarInfo*);
