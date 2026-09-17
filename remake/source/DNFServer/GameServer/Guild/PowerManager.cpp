// df_guild_r — PowerManager (split from source/guild per ORIG layout)
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

#include "PowerManager.h"
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
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

struct PowerManagerLayout
{
    char pad0[4];
    CApplication* m_app;   // +4
    char rest[0x184 - 8];
    char m_winnerSide;     // +0x184
    char rest2[0x198 - 0x185];
};

void CPowerManager::Process()
{
}

void CPowerManager::SetPowerDBFlag(unsigned short flag)
{
    m_powerDBFlag |= flag;
}

bool CPowerManager::IsPowerWarOn()
{
    return m_powerWar.IsPowerWarOn();
}

CPowerManager::CPowerManager()
    : m_app(0)
{
    m_winnerSide = 0;
    m_powerWarEndKillPoint = 0x3c;
    m_sideCount = 3;
    m_powerDBFlag = 0;
}

CPowerManager::~CPowerManager()
{
}

void CPowerManager::InitPowerManager(char* path, CApplication* app)
{
    ((PowerManagerLayout*)this)->m_app = app;
    LoadPowerWarCfg(path);
}

void CPowerManager::SetPowerInfo(char side, int score1, int score2)
{
    m_winnerSide = side;
    if (!IsPowerWarOn())
    {
        m_power[1].SetScore(score1);
        m_power[2].SetScore(score2);
    }
    SetPowerDBFlag(4);
}

void CPowerManager::SendPowerWarInfo()
{
    Packet_Notice_Power_War_Info pkt;
    pkt.m_winner = m_winnerSide;
    pkt.m_scoreA = m_power[1].GetScore();
    pkt.m_scoreB = m_power[2].GetScore();
    ((CApplication*)m_app)->Get_ServerHandler()->SendAllTcpGameServer(&pkt);
}

void CPowerManager::SendPowerWarScore()
{
    CServerHandler* handler;
    if ((handler = ((CApplication*)m_app)->Get_ServerHandler()) == 0)
    {
        throw CDNFException(
            "CGuildManager::OnChangePowerWarScore() pclServerHandler == NULL\n");
    }
    Packet_Reply_Power_War_Score pkt;
    pkt.m_scoreA = GetPowerScore((ENUM_POWER_SIDE_TYPE)1);
    pkt.m_scoreB = GetPowerScore((ENUM_POWER_SIDE_TYPE)2);
    handler->SendAllTcpGameServer(&pkt);
    DNF_LOG_SCOPE_LINE(0x75,"./log/Power", "Power Point : A(%d)  B(%d)",
        pkt.m_scoreA, pkt.m_scoreB);
}

void CPowerManager::SendPowerWarEndTime(int time)
{
    CServerHandler* handler;
    if ((handler = ((CApplication*)m_app)->Get_ServerHandler()) == 0)
    {
        throw CDNFException(
            "CGuildManager::OnChangePowerWarScore() pclServerHandler == NULL\n");
    }
    Packet_Notice_Power_War_End_Time pkt;
    pkt.m_time = (unsigned char)time;
    pkt.m_scoreA = GetPowerScore((ENUM_POWER_SIDE_TYPE)1);
    pkt.m_scoreB = GetPowerScore((ENUM_POWER_SIDE_TYPE)2);
    handler->SendAllUdpGameServer((char*)&pkt, 0x13);
}

int CPowerManager::IncPowerScore(ENUM_POWER_SIDE_TYPE side, int score)
{
    if (m_powerWar.IsPowerWarOn() != 0)
    {
        return m_power[side].IncScore(score);
    }
    return m_power[side].GetScore();
}

int CPowerManager::GetPowerScore(ENUM_POWER_SIDE_TYPE side)
{
    return m_power[side].GetScore();
}

void CPowerManager::StartPowerWarEvent()
{
    m_powerWar.CPowerWar::setEvent();
    for (int i = 0; i < 3; i++)
    {
        m_power[i].InitPower();
    }
    DNF_LOG_SCOPE_LINE(0xb9, "./log/Power", "CPowerManager::StartPowerWarEvent");
}

int CPowerManager::ComputeWinnerSide()
{
    if (GetPowerScore((ENUM_POWER_SIDE_TYPE)1) > GetPowerScore((ENUM_POWER_SIDE_TYPE)2))
    {
        return 1;
    }
    if (GetPowerScore((ENUM_POWER_SIDE_TYPE)1) < GetPowerScore((ENUM_POWER_SIDE_TYPE)2))
    {
        return 2;
    }
    return 0;
}

void CPowerManager::EndPowerWarEvent()
{
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0xd5, "./log/Power", "CPowerManager::EndPowerWarEvent TOP");
    SetWinnerSide(ComputeWinnerSide());
    RewardBonusPoint();
    SaveDBPowerWarPoint();
    CalcPowerWarRank(true);
    RewardGuildPowerWarPoint();
    PrintDebugInfo();
    SaveDBPowerWarRank();
    SendPowerWarInfo();
    SendPowerWarEndInfo();
    CleanPowerWar();
    m_powerWar.CPowerWar::resetEvent();
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0xfd, "./log/Power", "CPowerManager::EndPowerWarEvent BOTTOM");
}

void CPowerManager::CleanPowerWar()
{
    for (int i = 0; i < 3; i++)
    {
        m_power[i].CleanPower();
    }
    DNF_LOG_SCOPE_LINE(0x170, "./log/Power", "CleanPower");
}

void CPowerManager::CalcPowerWarRank(bool flag)
{
    DNF_LOG_SCOPE_LINE(0x17c, "./log/Power", "CPowerManager::CalcPowerWarRank(%s)",
        flag ? "All" : "Winner");
    if (flag)
    {
        m_power[1].CalcPowerWarRank();
        m_power[2].CalcPowerWarRank();
    }
    else if (m_winnerSide == 0 || m_winnerSide > 2)
    {
        DNF_LOG_SCOPE_LINE(0x187,"./log/Power", "invalid winner side income(%d)",
            (int)m_winnerSide);
    }
    else
    {
        m_power[m_winnerSide].CalcPowerWarRank();
    }
}

void CPowerManager::RewardGuildPowerWarPoint()
{
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x198, "./log/PowerResult", "CPowerManager::RewardGuildPowerWarPoint");
    if (m_winnerSide == 0 || m_winnerSide > 2)
    {
        DNF_LOG_SCOPE_LINE(0x19c, "./log/PowerResult", "invalid winner side income(%d)", (int)m_winnerSide);
        return;
    }
    m_power[m_winnerSide].RewardGuildPowerWarPoint(
        *((CApplication*)m_app)->Get_GuildManager(), true,
        m_reward1, m_reward2, m_reward3, m_reward4);
    if (m_winnerSide == 1)
    {
        m_power[2].RewardGuildPowerWarPoint(
            *((CApplication*)m_app)->Get_GuildManager(), false,
            m_reward1, m_reward2, m_reward3, m_reward4);
    }
    else
    {
        m_power[1].RewardGuildPowerWarPoint(
            *((CApplication*)m_app)->Get_GuildManager(), false,
            m_reward1, m_reward2, m_reward3, m_reward4);
    }
}

// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0016 | guild | 与ORIG差异=DIFF | CPowerManager::SaveDBPowerWarRank | 详见 function_reports/guild/_ZN13CPowerManager18SaveDBPowerWarRankEv.md
// 2026-09-15 HWSpecSend：最佳 ndiff=190（ours 394 / orig 388，较 GuildB base 230 改善 40，已保留其「删 winnerSide/app + 内联 m_winnerSide」轴）。
// 已穷举并劣化/无效的轴（勿重复）：W×A×S 三维 8 格矩阵（GuildB：230/232/384/392/477/485/379/387，全劣于 190 基线）；
// 局部上移 `users` 到函数首部=197；补 `((unsigned int*)noticePkt.m_data)[i]=*(unsigned int*)users.at(i)->m_data;`（双次 at(i)）+
// 日志第 3 实参改读 `statuePkt.m_rankers[i]` =410；两者组合=412；`int reservedSlot` 占位=512。
// 帧已与 ORIG 逐字节相同（`push %ebp; mov; push %esi; push %ebx; sub $0x1850,%esp`）⇒ 非帧轴问题；
// 槽集合为「两侧各有独占槽」的混合态（ORIG 独占 -0x34/-0x3c/-0x44/-0x54/-0x5c/-0x64/-0x74/-0x75/-0x7f/-0x878/-0x87c..-0x87f/-0x8b/-0x95/-0xa4，
// 我方独占 -0x28/-0x2d/-0x38/-0x48/-0x50/-0x58/-0x60/-0x70/-0x7c/-0x7d/-0x87/-0x884/-0x88a/-0x88b/-0x895/-0x9d/-0xac/-0xb0）
// ⇒ 按判据表属「反向平移/混合态」，占位与补语句均无效，登记为强耦合复合族。
// 2026-09-15 HWSpecSend（第二轮诊断，用 call 符号序列比对 —— 该方法不受错位假信号影响）：
//   ORIG 比本形态**多 1 次 `Packet_DB_Save_Power_War_Guild_Rank::C1Ev`** 与 **1 次 `vector::at`**（其余 20 个符号计数逐项相同）。
//   根因：ORIG 的两个 packet ctor 作用在**同一栈地址**（`lea -0x889(%ebp)` 后分别 call User_RankC1/Guild_RankC1）
//   ⇒ 原源码是「两个**独立作用域块**内的大局部」，由 4.1.2 系编译器**复用栈槽**（4.4.6 不合并，故我方用引用别名替代）。
//   三种复现尝试全部劣化并回退：①`{userPkt…}` 块 + 块外独立 `guildPkt` = .o 386；②仅独立 `guildPkt`（无块）= 386；
//   ③placement new（`::new ((void*)&userPkt) Packet_DB_Save_Power_War_Guild_Rank();` + 7 处 `->` + 1 处 SendToDB 实参）= **538**。
//   ⇒ 该 ctor/at 计数差异属「**编译器栈槽复用行为族**」，在本工具链下源码不可调；维持 190 为最佳形态，勿再试独立大局部。
void CPowerManager::SaveDBPowerWarRank()
{
    // [2026-09-15 GuildA] 「局部语句缺失 + 双向平移」复合族，二维矩阵实测（A 轴=内联重建期
    //   局部，B 轴=补 ORIG 的逐项写入）。全部 9 格 + 7 个扩展格均记录，最优 = A3B0（保留）：
    //        B0 不补语句 | B1 补两次 users.at(i) 写入 | B2 补写入(单次 at)
    //   A0 保留 winnerSide+app : 230 | 399 | 236
    //   A1 内联 winnerSide     : 477 | 393 |  —
    //   A2 内联 app            : 195 | 415 |  —
    //   A3 两者都内联          : 190 ← 保留 | 410 |  —
    //   扩展（A3 基）：内联 s1/s2=192、内联 group=190、内联 characInfo=319、
    //   内联 guildInfo=415、合并两处循环 side=390、side 改名对照=190。
    //   ORIG 真实形态（已逐条读出 0x80a4757..0x80a47b9）：对每个 i **调用 users.at(i) 两次**
    //   —— 第一次写 statuePkt.m_rankers[i](-0x74+i*4)，第二次写 noticePkt.m_data[i](-0x8b+i*4)，
    //   日志第 3 实参再读回 -0x74；补回该语句（B1）反而劣化 ⇒ 本件是「语句缺失 × 栈布局平移」
    //   的复合，单轴不可解（GuildB 单轴基线 399/236 与本矩阵一致）。
    //   保留最优形态后剩余 ndiff 190（orig 388 / ours 394），登记为复合族。
    if (m_winnerSide != 0 && m_winnerSide < 3)
    {
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x1b6, "./log/PowerResult", "POWER WAR RESULT DB SAVE START");
        {
        Packet_DB_Save_Power_War_User_Rank userPkt;
        unsigned char group = (*(CApplication**)&m_app)->Get_ServerHandler()->GetServerGroupNo();
        userPkt.m_b = group;
        STUserRank userRanks[500];
        for (int side = 1; side < 3; side++)
        {
            memset(userRanks, 0, 4000);
            userPkt.m_c = (unsigned char)side;
            CPowerWarCharacInfo* characInfo =
                m_power[side].GetPowerWarCharacInfo();
            unsigned int count = 0;
            characInfo->GetAllUserRankingInfo(count, userRanks);
            {
                DNF_LOG_SCOPE_LINE(0x1d1,"./log/PowerResult", "SaveDBPowerWarRank() PowerSide %d, User Rank Count is %d",
                    side, count);
            }
            if (count == 0)
            {
                DNF_LOG_SCOPE_LINE(0x1d6,"./log/PowerResult",
                    "SaveDBPowerWarRank() PowerSide %d, User Rank Count is 0", side, count);
            }
            else if (count < 0xfb)
            {
                userPkt.m_a = 1;
                userPkt.m_e = count;
                memcpy(userPkt.m_ranks, userRanks, count << 3);
                (*(CApplication**)&m_app)->Get_ServerHandler()->SendToDB(&userPkt);
            }
            else
            {
                userPkt.m_a = (unsigned char)(side == 1);
                userPkt.m_e = 0xfa;
                memcpy(userPkt.m_ranks, userRanks, 2000);
                (*(CApplication**)&m_app)->Get_ServerHandler()->SendToDB(&userPkt);
                userPkt.m_c = 0xfb;
                userPkt.m_a = 0;
                userPkt.m_e = count - 0xfa;
                memcpy(userPkt.m_ranks, (char*)userRanks + 2000, (count - 0xfa) * 8);
                (*(CApplication**)&m_app)->Get_ServerHandler()->SendToDB(&userPkt);
            }
        }
        }
        Packet_DB_Save_Power_War_Guild_Rank guildPkt;
        guildPkt.m_a = (*(CApplication**)&m_app)->Get_ServerHandler()->GetServerGroupNo();
        for (int side = 1; side < 3; side++)
        {
            guildPkt.m_b = (unsigned char)side;
            guildPkt.m_c = 0;
            memset(guildPkt.m_ranks, 0, 800);
            CPowerWarGuildInfo* guildInfo =
                m_power[side].GetPowerWarGuildInfo();
            guildInfo->GetAllGuildRankingInfo(
                *(int*)&guildPkt.m_c, (STGuildRank*)((char*)&guildPkt + 0x10));
            (*(CApplication**)&m_app)->Get_ServerHandler()->SendToDB(&guildPkt);
            {
                DNF_LOG_SCOPE_LINE(0x248,"./log/PowerResult",
                    "SaveDBPowerWarRank() SEND Packet_DB_Save_Power_War_Guild_Rank Power:%d", side);
            }
        }
        Packet_DB_Save_Power_War_Statue_Ranker statuePkt;
        Packet_Notice_Power_War_Rank noticePkt;
        statuePkt.m_group = (*(CApplication**)&m_app)->Get_ServerHandler()->GetServerGroupNo();
        std::vector<STPowerWarCharacInfo*> users;
        CPowerWarCharacInfo* s1 = m_power[1].GetPowerWarCharacInfo();
        CPowerWarCharacInfo* s2 = m_power[2].GetPowerWarCharacInfo();
        s1->GetStatueRankingUsers(users);
        s2->GetStatueRankingUsers(users);
        std::sort(users.begin(), users.end(), STPowerWarCharacInfo::Compare);
        for (int i = 0; i < 3; i++)
        {
            if (i < (int)users.size())
            {
                statuePkt.m_rankers[i] = *(unsigned int*)users.at(i)->m_data;
                ((unsigned int*)noticePkt.m_data)[i] = *(unsigned int*)users.at(i)->m_data;
                DNF_LOG_SCOPE_LINE(0x273, "./log/PowerResult", "Last Rank:%d, Charac No:%d", i, statuePkt.m_rankers[i]);
            }
        }
        {
            DNF_LOG_SCOPE_LINE(0x277,"./log/PowerResult",
                "SaveDBPowerWarRank() SEND Packet_DB_Save_Power_War_Statue_Ranker");
        }
        (*(CApplication**)&m_app)->Get_ServerHandler()->SendToDB(&statuePkt);
        {
            DNF_LOG_SCOPE_LINE(0x27b, "./log/PowerResult", "POWER WAR RESULT DB SAVE END");
        }
    }
    return;
}

void CPowerManager::SaveDBPowerWarPoint()
{
    Packet_DB_Save_Power_War_Point pkt;
    pkt.m_winner = GetWinnerSide();
    pkt.m_scoreA = GetPowerScore((ENUM_POWER_SIDE_TYPE)1);
    pkt.m_scoreB = GetPowerScore((ENUM_POWER_SIDE_TYPE)2);
    pkt.m_group = ((CApplication*)m_app)->Get_ServerGroup();
    ((CApplication*)m_app)->Get_ServerHandler()->SendToDB(&pkt);
    DNF_LOG_SCOPE_LINE(0x28e,"./log/Power", "winner:%d, A:%d, B:%d, svr group:%d",
        pkt.m_winner, pkt.m_scoreA, pkt.m_scoreB, (unsigned int)pkt.m_group);
}

void CPowerManager::LoadPowerWarCfg(char* path)
{
    m_powerWar.LoadPowerWarTableFile(path);
}

void CPowerManager::SetPowerWarEndKillPoint(unsigned short point)
{
    m_powerWar.setPowerWarEndKillPoint(point);
}

unsigned short CPowerManager::GetPowerWarEndKillPoint()
{
    return ((const CPowerWar&)m_powerWar).getPowerWarEndKillPoint();
}

void CPowerManager::ProcessByMinute()
{
}

void CPowerManager::RewardBonusPoint()
{
    if (m_winnerSide == 0 || m_winnerSide > 2)
    {
        return;
    }
    CPowerWarCharacInfo* info = m_power[m_winnerSide].GetPowerWarCharacInfo();
    info->CalcBonus();
}

void CPowerManager::SaveDBPowerWarBonusPoint()
{
    if (m_winnerSide == 0 || m_winnerSide > 2)
    {
        return;
    }
    Packet_DB_Save_Power_War_Bonus_Point pkt;
    CPowerWarCharacInfo* info = m_power[m_winnerSide].GetPowerWarCharacInfo();
    info->GetBonus(pkt);
    if (pkt.m_bonus != 0)
    {
        ((CApplication*)m_app)->Get_ServerHandler()->SendToDB(&pkt);
    }
}

void CPowerManager::SaveDBPowerWarPointReward()
{
    if (m_winnerSide == 0 || m_winnerSide > 2)
    {
        DNF_LOG_SCOPE_LINE(0x357, "./log/Power", "invalid winner side income(%d)", (int)m_winnerSide);
        return;
    }
    for (int side = 1; side < 3; side++)
    {
        CPowerWarGuildInfo* info = m_power[side].GetPowerWarGuildInfo();
        int count = info->GetPowerWarPointDBSaveCount();
        if (count != 0)
        {
            Packet_DB_Save_Power_War_Point_Reward pkt;
            pkt.m_a = ((CApplication*)m_app)->Get_ServerHandler()->GetServerGroupNo();
            info->MakePacketDBPowerWarPoint(&pkt);
            ((CApplication*)m_app)->Get_ServerHandler()->SendToDB(&pkt);
        }
    }
}

void CPowerManager::UpdatePowerWarInfo(bool flag, ENUM_POWER_SIDE_TYPE side, int score, unsigned int* p)
{
    if (m_app == 0)
    {
        return;
    }
    for (int i = 0; i <= 3; i++)
    {
        if (p[i] == 0)
        {
            continue;
        }
        CUser* user = ((CApplication*)m_app)->Get_UserManager()->FindUser_CharNo(p[i]);
        if (user == 0)
        {
            continue;
        }
        unsigned int guildKey = user->GetGuildKey();
        m_power[side].UpdatePowerWarInfo(score, p[i], guildKey);
    }
}

void CPowerManager::SetPowerWarRewardInfo(int a, int b, int c, int d)
{
    m_reward1 = a;
    m_reward2 = b;
    m_reward3 = c;
    m_reward4 = d;
}

unsigned int CPowerManager::GetUserPowerWarPoint(ENUM_POWER_SIDE_TYPE side, unsigned int charNo)
{
    return m_power[side].GetPowerWarCharacInfo()
        ->GetUserPowerWarPoint(charNo);
}

unsigned int CPowerManager::GetGuildRankingInPower(ENUM_POWER_SIDE_TYPE side, unsigned int guildKey)
{
    return m_power[side].GetPowerWarGuildInfo()
        ->GetGuildRanking(guildKey);
}

unsigned int CPowerManager::GetUserRankingInPower(ENUM_POWER_SIDE_TYPE side, unsigned int charNo)
{
    return m_power[side].GetPowerWarCharacInfo()
        ->GetUserRanking(charNo);
}

void CPowerManager::PrintDebugInfo()
{
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x3c9, "./log/PowerResult", "----- POWER A");
    m_power[1].GetPowerWarGuildInfo()->PrintDebugInfo();
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x3cf, "./log/PowerResult", "----- POWER B");
    m_power[2].GetPowerWarGuildInfo()->PrintDebugInfo();
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0017 已消除）| CPowerManager::SendPowerWarProcessInfo
// 2026-09-15 HWSpecSend：已归零（ndiff 5→0）。修法 = `unsigned int guildKey` → `int guildKey = (int)user->GetGuildKey();`
// （P2/U4 signedness 族：目标类型决定跨调用保活寄存器与载入序；`register unsigned int` 反而 28、`const`/cast-to-uint 均 5）。
void CPowerManager::SendPowerWarProcessInfo(unsigned int charNo)
{
    CUser* user =
        ((CApplication*)m_app)->Get_UserManager()->FindUser_CharNo(charNo);
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x3e2,"./log/Power", "CPacketTranslater::SendPowerWarProcessInfo : 0 == pclUser(%d)",
            charNo);
        return;
    }
    int guildKey = (int)user->GetGuildKey();
    CGuild* guild = ((CApplication*)m_app)->Get_GuildManager()->FindGuild(guildKey);
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x3ea,"./log/Power",
            "CPacketTranslater::SendPowerWarProcessInfo : 0 == pclGuild(%d), pclUser(%d)",
            guildKey, charNo);
        return;
    }
    Packet_Channel_Power_War_Process_Info pkt;
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_charNo = charNo;
    pkt.m_scoreA = GetPowerScore((ENUM_POWER_SIDE_TYPE)1);
    pkt.m_scoreB = GetPowerScore((ENUM_POWER_SIDE_TYPE)2);
    int side = (int)(unsigned char)guild->GetPowerSide();
    if (side == 3)
    {
        side = 1;
    }
    else if (side == 4)
    {
        side = 2;
    }
    else if (side == 0 || 4 < side)
    {
        DNF_LOG_SCOPE_LINE(0x3fd,"./log/Power", "CPacketTranslater::SendPowerWarProcessInfo : Invalid Power Side(%d)",
            (int)(unsigned char)guild->GetPowerSide());
        return;
    }
    pkt.m_userPoint = GetUserPowerWarPoint((ENUM_POWER_SIDE_TYPE)side, charNo);
    if (IsPowerWarOn())
    {
        pkt.m_guildRank = GetGuildRankingInPower((ENUM_POWER_SIDE_TYPE)side, guildKey);
        pkt.m_userRank = GetUserRankingInPower((ENUM_POWER_SIDE_TYPE)side, charNo);
    }
    else
    {
        pkt.m_guildRank = 0;
        pkt.m_userRank = 0;
    }
    user->SendToGameserver((char*)&pkt, 0x26);
}

void CPowerManager::SendPowerWarEndInfo(int time)
{
}

void CPowerManager::SendPowerWarEndInfo()
{
    DNF_LOG_SCOPE_LINE(0x419, "./log/PowerResult", "SEND POWER WAR END INFO START");
    SendPowerWarEndInfoInSpecificPower(1);
    SendPowerWarEndInfoInSpecificPower(2);
    DNF_LOG_SCOPE_LINE(0x41f, "./log/PowerResult", "SEND POWER WAR END INFO END");
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0018 已消除）| CPowerManager::SendPowerWarEndInfoInSpecificPower
void CPowerManager::SendPowerWarEndInfoInSpecificPower(char side)
{
    DNF_LOG_SCOPE_LINE(0x42a, "./log/Power", "SEND POWER WAR END INFO %d Power START", side);
    CUser* user = 0;
    CGuild* guild = 0;
    unsigned char isWinner = 0;
    unsigned int charNo = 0;
    unsigned int userRank = 1;
    unsigned int guildRank = 0;
    unsigned int guildKey = 0;
    STPowerWarCharacInfo* characInfo = 0;
    STPowerWarGuildInfo* guildInfo = 0;
    std::vector<STPowerWarCharacInfo*>* vec =
        m_power[side].GetPowerWarCharacInfo()
            ->GetCharacInfoVector();
    std::vector<STPowerWarCharacInfo*>::iterator it = vec->begin();
    {
        DNF_LOG_SCOPE_LINE(0x439, "./log/PowerResult", "SORT USER RANK COUNT : %d",
            (int)vec->size());
    }
    for (; it != vec->end(); ++it)
    {
        characInfo = *it;
        if (characInfo != 0)
        {

            CUser* user = ((CApplication*)m_app)->Get_UserManager()
                ->FindUser_CharNo(*(unsigned int*)characInfo->m_data);
            if (user != 0)
            {
                guild = ((CApplication*)m_app)->Get_GuildManager()
                    ->FindGuild(user->GetGuildKey());
                if (guild != 0)
                {
                    charNo = user->GetUniqCharNo();
                    guildKey = user->GetGuildKey();
                    isWinner = (guild->GetPowerSide() == GetWinnerSide());
                    guildRank = m_power[side].GetPowerWarGuildInfo()->GetGuildRanking(guildKey);
                    guildInfo = m_power[side].GetPowerWarGuildInfo()->GetSpecificGuildInfo(guildKey);
                    if (guildInfo != 0)
                    {
                        SendPowerWarEndInfoToSpecificUser(user, charNo, isWinner,
                            characInfo->m_field[1], userRank,
                            guildInfo->m_field[1],
                            guildInfo->m_field[2], guildRank);
                    }
                    else
                    {
                        DNF_LOG_SCOPE_LINE(0x45c, "./log/PowerResult",
                            "DBID(%d) CharacNo(%d) UserPP(%d) UserRank(%d)",
                            user->GetDBID(), charNo, characInfo->m_field[1],
                            userRank);
                    }
                }
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x463, "./log/PowerResult", "CharacInfo is NULL");
        }

        userRank++;
    }
    {
        DNF_LOG_SCOPE_LINE(0x46b, "./log/PowerResult", "SEND POWER WAR END INFO %d Power END", side);
    }
}

void CPowerManager::SendPowerWarEndInfoToSpecificUser(CUser* user, unsigned int b,
                                                     unsigned char c, unsigned int d,
                                                     unsigned int e, unsigned int f,
                                                     unsigned int g, unsigned int h)
{
    Packet_Notice_Power_war_End_Info pkt;
    CPowerManager* powerManager = ((CApplication*)m_app)->GetPowerManager();
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_b = b;
    pkt.m_c = c;
    pkt.m_d = d;
    pkt.m_e = e;
    pkt.m_f = f;
    pkt.m_g = g;
    pkt.m_h = h;
    user->SendToGameserver((char*)&pkt, 0x27);
}

STUserPoint::STUserPoint()
    : m0(0), m4(0)
{
}

Packet_DB_Save_Power_War_Bonus_Point::Packet_DB_Save_Power_War_Bonus_Point()
    : PacketHeader(0x6f4, 0x7de), m_bonus(0)
{
    memset(m_points, 0, sizeof(m_points));
}

Packet_DB_Save_Power_War_Point::Packet_DB_Save_Power_War_Point()
    : PacketHeader(0x6ad, 0x14)
{
    (void)0;
}

STGuildRank::STGuildRank()
    : m0(0), m4(0)
{
}

Packet_DB_Save_Power_War_Guild_Rank::Packet_DB_Save_Power_War_Guild_Rank()
    : PacketHeader(0x6d6, 0x330), m_a(0), m_b(0), m_c(0)
{
    memset(m_ranks, 0, sizeof(m_ranks));
}

STUserRank::STUserRank()
    : m0(0), m4(0)
{
}

Packet_DB_Save_Power_War_User_Rank::Packet_DB_Save_Power_War_User_Rank()
    : PacketHeader(0x6d7, 0x7e5), m_a(0), m_b(0), m_c(0), m_d(0), m_e(0)
{
    memset(m_ranks, 0, sizeof(m_ranks));
}

Packet_DB_Save_Power_War_Statue_Ranker::Packet_DB_Save_Power_War_Statue_Ranker()
    : PacketHeader(0x6da, 0x17)
{
    m_group = 0;
    memset(m_rankers, 0, sizeof(m_rankers));
}

STPowerWarPointInfo::STPowerWarPointInfo()
    : m0(0), m4(0)
{
}

Packet_DB_Save_Power_War_Point_Reward::Packet_DB_Save_Power_War_Point_Reward()
    : PacketHeader(0x6d9, 0x7df), m_a(0), m_b(0)
{
    memset(m_points, 0, sizeof(m_points));
}

#pragma pack(push,1)
struct Packet_Channel_Power_War_Process_Info_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned int m12;
    unsigned int m16;
    unsigned int m1a;
    unsigned int m1e;
    unsigned int m22;
};
#pragma pack(pop)
Packet_Channel_Power_War_Process_Info::Packet_Channel_Power_War_Process_Info()
    : PacketHeader(0x6e0, 0x26)
{
    ((Packet_Channel_Power_War_Process_Info_Layout*)this)->ma = 4294967295;
    ((Packet_Channel_Power_War_Process_Info_Layout*)this)->me = 0;
    ((Packet_Channel_Power_War_Process_Info_Layout*)this)->m12 = 0;
    ((Packet_Channel_Power_War_Process_Info_Layout*)this)->m16 = 0;
    ((Packet_Channel_Power_War_Process_Info_Layout*)this)->m1a = 0;
    ((Packet_Channel_Power_War_Process_Info_Layout*)this)->m1e = 0;
    ((Packet_Channel_Power_War_Process_Info_Layout*)this)->m22 = 0;
}

Packet_Notice_Power_War_Info::Packet_Notice_Power_War_Info()
    : PacketHeader(0x6ae, 0x13)
{
    (void)0;
}

Packet_Notice_Power_War_Rank::Packet_Notice_Power_War_Rank()
    : PacketHeader(0x6d8, 0x16)
{
    memset(m_data, 0, 0xc);
}

#pragma pack(push,1)
struct Packet_Notice_Power_war_End_Info_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned char m12;
    unsigned int m13;
    unsigned int m17;
    unsigned int m1b;
    unsigned int m1f;
    unsigned int m23;
};
#pragma pack(pop)
Packet_Notice_Power_war_End_Info::Packet_Notice_Power_war_End_Info()
    : PacketHeader(0x6b0, 0x27)
{
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->ma = 4294967295;
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->me = 0;
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->m12 = 0;
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->m13 = 0;
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->m17 = 0;
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->m1b = 0;
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->m1f = 0;
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->m23 = 0;
}

Packet_Notice_Power_War_End_Time::Packet_Notice_Power_War_End_Time()
    : PacketHeader(0x6ac, 0x13)
{
    (void)0;
}

#pragma pack(push,1)
struct Packet_Reply_Power_War_Score_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
};
#pragma pack(pop)
Packet_Reply_Power_War_Score::Packet_Reply_Power_War_Score()
    : PacketHeader(0x6a5, 0x12)
{
    ((Packet_Reply_Power_War_Score_Layout*)this)->ma = 0;
    ((Packet_Reply_Power_War_Score_Layout*)this)->me = 0;
}

char CPowerManager::GetWinnerSide()
{
    return m_winnerSide;
}

void CPowerManager::SetWinnerSide(char side)
{
    m_winnerSide = side;
}
