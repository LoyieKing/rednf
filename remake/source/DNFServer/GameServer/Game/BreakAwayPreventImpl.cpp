// ============================================================================
// df_game_r 还原 —— break_away_prevent::CBreakAwayPreventSystem 实现批次
// 权威头 Game/BreakAwayPreventSystem.h（GlobalData.cpp ctor/dtor/loadDbData、
// W6_CeraShopDeps.cpp updateUsedCera 归属不变）。
//
// 已完整阅读的报告（docs/class_func_reports/break_away_prevent__CBreakAwayPreventSystem/）：
//   getDiffConnectDay.md / calBreakAwayUserPoint.md / selectReward.md /
//   startBreakAwayPreventSystem.md / updateUvList.md /
//   loadDbData.md / CBreakAwayPreventSystem.md / ~CBreakAwayPreventSystem.md /
//   updateUsedCera.md
// 本 TU 落地（ORIG nm -S --defined-only 核实地址/大小）：
//   getDiffConnectDay              T 0x0831c794 (0x7b)
//   calBreakAwayUserPoint          T 0x0831c810 (0x61)
//   selectReward                   T 0x0831c872 (0x1e7)
//   updateUvList                   T 0x0831cb56 (0x308)
//   startBreakAwayPreventSystem    T 0x0831ce78 (0x2ec)
//   DB_UpdateBreakAwayUvList::makeRequest T 0x0843873a (0x107)
//     —— 独立报告缺失，按指南 §3.2 以 objdump 区间转写并标注。
//
// ⚠ 需 CMake 登记（新建 TU，加入 game 目标源列表）。
// ============================================================================

#include "BreakAwayPreventSystem.h"
#include "CUser.h"

#include "SigTypes.h"
#include "GlobalData.h"
#include "DB_UpdateGiftCount.h"      // DB_UpdateBreakAwayUvList 权威声明


char* NumberToString(unsigned int value, int radix);   // ORIG T 0x0810904b

long getPrevScheduleTime(int hour, long time);         // ORIG T 0x0846c2e9（GameUtils.cpp）
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "LogManager.h"
#include "InterDispatcher.h"   // Inter_LoadEtc::giveBreakAwayReward（giveBreakAwayDungeonClearReward）

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>


namespace break_away_prevent
{

// ---- getDiffConnectDay（ORIG 0x0831c794）-----------------------------------
// timeSec==0 直接返 0；否则按当日 0 点（getPrevScheduleTime(hour=0)）差折算天，
// 与 curSec 差/0x15180 后取 max(0, days)。asm 的 magic-multiply 为除以 0x15180。
unsigned int CBreakAwayPreventSystem::getDiffConnectDay(unsigned int timeSec)
{
    if (timeSec == 0)
        return 0;

    long prevDaySec = getPrevScheduleTime(0, static_cast<long>(timeSec));
    // ORIG 0x0831c794：getCurSec() 结果不落栈、除法结果直接入
    // max<unsigned int>(0, ·)（_ZSt3maxIjE）；负天数按 unsigned 包装返回
    // （int 解释为负），不可用 max<long> 钳制为 0。
    return std::max<unsigned int>(
        0u, static_cast<unsigned int>(
                (static_cast<long>(GlobalData::s_systemTime_.getCurSec())
                 - prevDaySec) /
                0x15180L));
}

// ---- calBreakAwayUserPoint（ORIG 0x0831c810）-------------------------------
// UV 名单 idx∈[0xf,0x1c]：'1'(0x31) 计数恰好 1 且落在 (0x15,0x1c] 高位区即命中。
bool CBreakAwayPreventSystem::calBreakAwayUserPoint()
{
    int lowCnt = 0;
    unsigned int highCnt = 0;

    for (unsigned int idx = 0xf; idx <= 0x1c; ++idx)
    {
        if (m_tail[idx] == 0x31)
        {
            if (idx > 0x15)
                ++highCnt;
            else
                ++lowCnt;
        }
    }
    if (highCnt != 1)
        return false;
    return true;
}

// ---- selectReward（ORIG 0x0831c872）----------------------------------------
// key=m_keys[keyIndex] 查 m_rewardMap；keyIndex∈{0,2,4}（startBreakAway 三分支）。
// point<0 返 0；遍历 list：
//   rewards ← {item.m_field8, m_fieldc, m_field10, point}（每轮重写暂存）
//   if (point < item.m_field0) return false;
//   if (item.m_field0 <= point <= item.m_field4) set + return true;
// 循环耗尽：仍按最后一轮暂存值调用 set 并返回 1（ORIG asm fallthrough 行为）。
bool CBreakAwayPreventSystem::selectReward(int keyIndex, int point, CUser* user)
{
    std::map<STRewardDataKey, STRewardDataList>::const_iterator it =
        m_rewardMap.find(m_keys[keyIndex]);
    if (it == m_rewardMap.end())
        return false;
    if (point < 0)
        return false;

    STBreakAwayRewards rewards;   // ORIG STBreakAwayRewards ctor 清零
    std::list<STRewardData>::const_iterator li;
    for (li = it->second.m_list.begin(); li != it->second.m_list.end(); ++li)
    {
        const STRewardData& entry = *li;
        rewards.m_rewardItemId = entry.m_field8;
        rewards.m_rewardAItemCnt = entry.m_fieldc;
        rewards.m_luckPoint = entry.m_field10;
        rewards.m_fieldc = point;
        if (point < entry.m_field0)
            return false;
        if (entry.m_field0 <= point && point <= entry.m_field4)
        {
            user->setBreakAwayRewards(rewards);
            return true;
        }
    }
    user->setBreakAwayRewards(rewards);
    return true;
}

// ---- updateUvList（ORIG 0x0831cb56）----------------------------------------
bool CBreakAwayPreventSystem::updateUvList(SIG_LOGIN_DATA const* login)
{
    int days =
        static_cast<int>(getDiffConnectDay(login->m_breakAwayLastUpdateTime));
    if (days < 0)
        days = 0;

    int lastPlayDays =
        static_cast<int>(getDiffConnectDay(login->m_lastPlayTime));

    if (days == 0)
    {
        // last_update_time 非 0 且当日：直接镜像登录包 play_info，不落 DB 请求。
        if (login->m_breakAwayLastUpdateTime != 0)
        {
            memset(m_tail, 0, sizeof(m_tail));
            memcpy(m_tail, login->m_breakAwayPlayInfo, 0x1f);
            return true;
        }
        days = lastPlayDays != 0 ? lastPlayDays : 1;
    }

    if (days >= 0x1e)
    {
        // 断线 ≥30 天：名单全 '0'（0x30），尾位标记 '1'，清写后请求落库。
        memset(m_tail, 0x30, sizeof(m_tail));
        m_tail[0x1d] = 0x31;   // this+0x328：名单第 29 位（高位区间标记）
        m_tail[0x1e] = '\0';   // this+0x329
        DB_UpdateBreakAwayUvList::makeRequest(login->m_accId, m_tail);
        return true;
    }

    std::string uvInfo(reinterpret_cast<const char*>(login->m_breakAwayPlayInfo));
    std::string uvMarked(uvInfo, days, 0x1e);
    for (int i = 0; i < days - 1; ++i)
        uvMarked.push_back('0');
    uvMarked.push_back('1');

    if (uvMarked.size() == 0x1e)
    {
        memset(m_tail, 0, sizeof(m_tail));
        strncpy(m_tail, uvMarked.c_str(), uvMarked.size());
        DB_UpdateBreakAwayUvList::makeRequest(login->m_accId, m_tail);
        return true;
    }

    LogManager::logFormat(
        1, "BreakAwayPreventSystem.cpp",
        __PRETTY_FUNCTION__,
        0x11c, "BREAK_AWAY : UV_INFO SIZE ERROR!! M_ID(%s)",
        NumberToString(login->m_accId, 0));
    return false;
}

// ---- startBreakAwayPreventSystem（ORIG 0x0831ce78）-------------------------
namespace {

// 三窗口共用奖励判定：accure_cera<0 → luck_point==0 时置 reward=false；
// 否则 selectReward(keyIdx,...) 成功后置 reward=true，order 给定时回写序号。
void applyWindowReward(CBreakAwayPreventSystem* sys,
                       SIG_LOGIN_DATA const* login, CUser* user,
                       int keyIndex, bool writeOrder, ENUM_BREAK_AWAY_REWARD_ORDER order)
{
    if (login->m_breakAwayAccureCera < 0)
    {
        if (login->m_breakAwayLuckPoint == 0)
            user->setBreakAwayRewardData(false);
        return;
    }
    if (!sys->selectReward(keyIndex, login->m_breakAwayAccureCera, user))
        return;
    user->setBreakAwayRewardData(true);
    if (writeOrder)
        user->setBreakAwayRewardOrder(order);
}

} // namespace

void CBreakAwayPreventSystem::startBreakAwayPreventSystem(
    SIG_LOGIN_DATA const* login, CUser* user)
{
    int dayDiff =
        static_cast<int>(getDiffConnectDay(login->m_breakAwayLastRewardTime));

    // 基类区未还原槽位【ORIG asm 直读，语义标推断】：
    //   +0x0c short：断线回归天数比较上限；+0x302/+0x303、+0x308/+0x309
    //   signed char：两奖励窗口的 diff-day 下界/上界。
    short dayLimit = *reinterpret_cast<const short*>((char*)this + 0xc);
    signed char winALow = *reinterpret_cast<const signed char*>((char*)this + 0x302);
    signed char winAHigh = *reinterpret_cast<const signed char*>((char*)this + 0x303);
    signed char winBLow = *reinterpret_cast<const signed char*>((char*)this + 0x308);
    signed char winBHigh = *reinterpret_cast<const signed char*>((char*)this + 0x309);

    unsigned int firstReward = login->m_breakAwayRewardBoundA;   // +0x3f40
    unsigned int secondReward = login->m_breakAwayRewardBoundB;  // +0x3f44
    bool questFieldsIdle = login->m_breakAwayDungeonClearCnt == 0 &&
                           login->m_breakAwayQuestTime != 0;

    // A 窗口：无归还记录或回归天数越过上限 → luck_point 主通道。
    if (login->m_breakAwayLastRewardTime == 0 || dayLimit <= dayDiff)
    {
        if (calBreakAwayUserPoint())
            applyWindowReward(this, login, user, 0, true,
                              static_cast<ENUM_BREAK_AWAY_REWARD_ORDER>(1));
        return;
    }

    // 区间 1：second < first（u32 比较）且 dayDiff ∈ [winA_low, win_a_high] → order 3。
    if (questFieldsIdle && secondReward < firstReward &&
        winALow <= dayDiff && dayDiff <= winAHigh)
    {
        applyWindowReward(this, login, user, 2, true,
                          static_cast<ENUM_BREAK_AWAY_REWARD_ORDER>(3));
        return;
    }

    // 区间 2：first < second 且 dayDiff ∈ [winB_low, win_b_high] → 只置 reward 标志。
    if (questFieldsIdle && firstReward < secondReward &&
        winBLow <= dayDiff && dayDiff <= winBHigh)
    {
        applyWindowReward(this, login, user, 4, false,
                          static_cast<ENUM_BREAK_AWAY_REWARD_ORDER>(0));
    }
}

} // namespace break_away_prevent

// ---- DB_UpdateBreakAwayUvList::makeRequest（ORIG 0x0843873a；报告缺失，objdump
//      逐指令转写）------------------------------------------------------------
void DB_UpdateBreakAwayUvList::makeRequest(unsigned int accId, char* uvInfo)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8622);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x147);             // opcode（DBThread 分发表）
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);
    SIG_BREAK_AWAY_UV_LIST* buf =
        guard.operator->()->GetInBuffer<SIG_BREAK_AWAY_UV_LIST>();
    strncpy(buf->m_pad, uvInfo, 0x1f);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// LinkFinal2 核销 —— break_away_prevent::CBreakAwayPreventSystem::GM_UpdateUvList
// （ORIG T @0x0831ca5a，0xfb）objdump 逐指令转写。
//   this+0x30b UV 名单缓冲（0x1f 字节）：先清零，再按 day 选择预设表：
//     day 1/2/3 → 0x8c242c0（31 字节字符串 "0000…111000010001"）
//     day 4     → 0x8c242e0（"0000…111000001000"）
//   然后 DB_UpdateBreakAwayUvList::makeRequest(accId, this+0x30b) +
//   CLocalChina_DB_BreakAwayReset::makeRequest(accId)；恒返 1。
// 同 TU 补齐两个 CLocalChina DB makeRequest（成员 mangled 与基类无关，
// W8LT_DBTask.cpp 的 DBDispatcher 派生类仅提供 C1/D1/vtable）：
//   CLocalChina_DB_BreakAwayReset::makeRequest(uint)    ORIG T 0x08127f42 (0xd7)
//   CLocalChina_DB_LastDay_BreakAway::makeRequest(uint,int,int) ORIG T 0x08128152 (0x10b)
// ============================================================================

class CLocalChina_DB_BreakAwayReset
{
public:
    static void makeRequest(unsigned int accId);
};

class CLocalChina_DB_LastDay_BreakAway
{
public:
    // ORIG 0x08128152 无 this：三参落 ebp+0x08/0x0c/0x10 → 静态成员
    // （静态/非静态成员 mangled 相同：_ZN32...11makeRequestEjii）。
    static void makeRequest(unsigned int accId, int day, int flag);
};

void CLocalChina_DB_BreakAwayReset::makeRequest(unsigned int accId)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localchina/localchina_DBThread.cpp", 0x408), true);
    guard.operator*()->operator<<(0x2dd);   // ORIG opcode（localchina DBThread 分发表）
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);   // ORIG operator<<(unsigned)
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void CLocalChina_DB_LastDay_BreakAway::makeRequest(unsigned int accId,
                                                   int day, int flag)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localchina/localchina_DBThread.cpp", 0x438), true);   // ORIG 字符串 .cpp
    guard.operator*()->operator<<(0x2de);  // ORIG opcode
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);  // ORIG operator<<(unsigned)（ebp+0x08）
    guard.operator*()->operator<<(flag);   // ORIG operator<<(int)（ebp+0x10，第三参）
    guard.operator*()->operator<<(day);    // ORIG operator<<(int)（ebp+0x0c，第二参）
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

namespace break_away_prevent
{

static const char* const kUvListDay123 = "000000000000000000111000010001";  // 0x8c242c0
static const char* const kUvListDay4   = "000000000000000000111000001000";  // 0x8c242e0

void CBreakAwayPreventSystem::GM_UpdateUvList(unsigned int accId,
                                              unsigned int day)
{
    memset((char*)this + 0x30b, 0, 0x1f);   // ORIG memset(this+0x30b,0,0x1f)
    switch (day)                        // ORIG switch ebp+0x10
    {
    case 1:
    case 2:
    case 3:
        memcpy((char*)this + 0x30b, kUvListDay123, 0x1f);   // ORIG 0x8c242c0
        break;
    case 4:
        memcpy((char*)this + 0x30b, kUvListDay4, 0x1f);     // ORIG 0x8c242e0
        break;
    default:
        break;   // ORIG default 无 memcpy
    }
    DB_UpdateBreakAwayUvList::makeRequest(
        accId, (char*)this + 0x30b);   // ORIG 0x0831cb3e
    CLocalChina_DB_BreakAwayReset::makeRequest(accId);   // ORIG static (no this) 0x0831cb49
}
// ---- StubS3A_deps 批 ----
// ORIG T 0x0831d164（报告 break_away_prevent__CBreakAwayPreventSystem/
// giveBreakAwayDungeonClearReward.md 全文转写）：order==1 → selectReward(1,
// accCera, user)；否则 selectReward(3, accCera, user)；selectReward 命中
// （返回非 0）则 Inter_LoadEtc::giveBreakAwayReward(user)。
void CBreakAwayPreventSystem::giveBreakAwayDungeonClearReward(CUser* user)
{
    int order = user->getBreakAwayRewardOrder();     // ORIG 0x0831d4f0
    if (order == 1)
    {
        int accCera = user->getBreakAwayAccureCera();   // ORIG 0x0831d4fe
        if (selectReward(1, accCera, user))
            Inter_LoadEtc::giveBreakAwayReward(user);   // ORIG 0x084d8e68
    }
    else
    {
        int accCera = user->getBreakAwayAccureCera();
        if (selectReward(3, accCera, user))
            Inter_LoadEtc::giveBreakAwayReward(user);
    }
}

}  // namespace break_away_prevent
