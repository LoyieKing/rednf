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
    // b28c-28（P0 槽-局部双向对应律）：ORIG 0x0831c806 段为
    // `mov (%eax),%eax; mov %eax,-0xc(%ebp); mov -0xc(%ebp),%eax`——
    // max 结果有**具名局部槽往返**（⇒ 原码非直 return 表达式），
    // 直 return 时 4.4 折成 `mov (%eax),%eax; leave` 少 2 条。
    // 负天数按 unsigned 包装返回（int 解释为负），不可用 max<long> 钳制为 0。
    unsigned int days = std::max<unsigned int>(
        0u, static_cast<unsigned int>(
                (static_cast<long>(GlobalData::s_systemTime_.getCurSec())
                 - prevDaySec) /
                0x15180L));
    return days;
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
void CBreakAwayPreventSystem::startBreakAwayPreventSystem(
    SIG_LOGIN_DATA const* login, CUser* user)
{
    int dayDiff =
        static_cast<int>(getDiffConnectDay(login->m_breakAwayLastRewardTime));

    // §4.3 具名化：+0x0c = 基类 SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER::m_fieldC
    // （unsigned short，SigTypes_A.h:470）。ORIG T 0x0831ce78
    // startBreakAwayPreventSystem 汇编 @+0x021..+0x029：
    //   `mov 0x8(%ebp),%eax` / `movzwl 0xc(%eax),%eax` / `cwtl` /
    //   `mov %eax,-0x10(%ebp)` ⇒ 无符号 16 位装载 + 显式 (short) 拓宽落 int 槽
    //   （有槽 ⇒ 原码确有具名局部，P0），故写 `int dayLimit = (short)m_fieldC;`。
    int dayLimit = (short)m_fieldC;
    // §4.3 具名化（+0x302/0x303/0x308/0x309 裸偏移清零）：+0x2fc 起为
    // m_keys[5]（STRewardDataKey 3 字节紧排，见 BreakAwayPreventSystem.h 布局
    // 注释），故 +0x302/0x303 = m_keys[2].m_data[0]/[1]、+0x308/0x309 =
    // m_keys[4].m_data[0]/[1]。ORIG T 0x0831ce78 对这四个字节是**无栈槽**的
    // 内联直读（@0x0831cfad `movzx eax,BYTE PTR [eax+0x302]` + `movsx eax,al`
    // 后紧接 `cmp eax,[ebp-0x14]`=dayDiff；另三点 +0x303/@0x0831cfc3、
    // +0x308/@0x0831d0ae、+0x309/@0x0831d0c4 同形）⇒ 按 P0 原码无对应局部，
    // 故读取写成条件内右值（原 `signed char winALow=…` 四个具名槽系重建侧
    // 发明）；`(signed char)(unsigned char)` 双步保 movzx→movsx 有符号日界
    // 语义（非 P196 反演）。实测 ndiff 263→186、verdict 桶不变。

    unsigned int firstReward = login->m_breakAwayRewardBoundA;   // +0x3f40
    unsigned int secondReward = login->m_breakAwayRewardBoundB;  // +0x3f44
    bool questFieldsIdle = login->m_breakAwayDungeonClearCnt == 0 &&
                           login->m_breakAwayQuestTime != 0;

    // A 窗口：无归还记录或回归天数越过上限 → luck_point 主通道。
    if (login->m_breakAwayLastRewardTime == 0 || dayLimit <= dayDiff)
    {
        if (calBreakAwayUserPoint())
        {   // ORIG 内联（applyWindowReward key=0 order=1 writeOrder）
            if (login->m_breakAwayAccureCera < 0)
            {
                if (login->m_breakAwayLuckPoint == 0)
                    user->setBreakAwayRewardData(false);
            }
            else if (selectReward(0, login->m_breakAwayAccureCera, user))
            {
                user->setBreakAwayRewardData(true);
                user->setBreakAwayRewardOrder(
                    static_cast<ENUM_BREAK_AWAY_REWARD_ORDER>(1));
            }
        }
        return;
    }

    // 区间 1：second < first（u32 比较）且 dayDiff ∈ [winA_low, win_a_high] → order 3。
    if (questFieldsIdle && secondReward < firstReward &&
        (signed char)(unsigned char)m_keys[2].m_data[0] <= dayDiff &&
        dayDiff <= (signed char)(unsigned char)m_keys[2].m_data[1])
    {
        if (login->m_breakAwayAccureCera < 0)
        {   // ORIG 内联（applyWindowReward key=2 order=3 writeOrder）
            if (login->m_breakAwayLuckPoint == 0)
                user->setBreakAwayRewardData(false);
        }
        else if (selectReward(2, login->m_breakAwayAccureCera, user))
        {
            user->setBreakAwayRewardData(true);
            user->setBreakAwayRewardOrder(
                static_cast<ENUM_BREAK_AWAY_REWARD_ORDER>(3));
        }
        return;
    }

    // 区间 2：first < second 且 dayDiff ∈ [winB_low, win_b_high] → 只置 reward 标志。
    if (questFieldsIdle && firstReward < secondReward &&
        (signed char)(unsigned char)m_keys[4].m_data[0] <= dayDiff &&
        dayDiff <= (signed char)(unsigned char)m_keys[4].m_data[1])
    {
        if (login->m_breakAwayAccureCera < 0)
        {   // ORIG 内联（applyWindowReward key=4 无 order 写）
            if (login->m_breakAwayLuckPoint == 0)
                user->setBreakAwayRewardData(false);
        }
        else if (selectReward(4, login->m_breakAwayAccureCera, user))
        {
            user->setBreakAwayRewardData(true);
        }
    }
}

} // namespace break_away_prevent

// ---- DB_UpdateBreakAwayUvList::makeRequest（ORIG 0x0843873a；报告缺失，objdump
//      逐指令转写）------------------------------------------------------------
// [b28c-31] AE：Acquire 结果实参内联复现 ORIG 0x0843874x 寄存器直传（70=70）。
void DB_UpdateBreakAwayUvList::makeRequest(unsigned int accId, char* uvInfo)
{
    // [b28c-31] ORIG 0x0843874x：Acquire 结果寄存器直传 CStreamGuard::C1（无栈槽往返，
    // P0/P89）⇒ 实参内联，勿命名 stream 局部。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8622), true);
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

int CBreakAwayPreventSystem::GM_UpdateUvList(unsigned int accId,
                                             unsigned int day)
{
    memset(m_tail, 0, 0x1f);   // ORIG memset(this+0x30b,0,0x1f)
    // ORIG 决策树（0x0831ca80..0x0831cb27）：day 1/2/3 各独立 memcpy 同串
    // （0x8c242c0），day 4 用另一串（0x8c242e0），default 无 memcpy。
    switch (day)                        // ORIG switch ebp+0x10
    {
    case 1:
        memcpy(m_tail, kUvListDay123, 0x1f);   // ORIG 块 @0x4c 0x8c242c0
        break;
    case 2:
        memcpy(m_tail, kUvListDay123, 0x1f);   // ORIG 块 @0x6e 0x8c242c0
        break;
    case 3:
        memcpy(m_tail, kUvListDay123, 0x1f);   // ORIG 块 @0x90 0x8c242c0
        break;
    case 4:
        memcpy(m_tail, kUvListDay4, 0x1f);     // ORIG 块 @0xb2 0x8c242e0
        break;
    default:
        break;   // ORIG default 无 memcpy
    }
    DB_UpdateBreakAwayUvList::makeRequest(
        accId, m_tail);   // ORIG 0x0831cb3e
    CLocalChina_DB_BreakAwayReset::makeRequest(accId);   // ORIG static (no this) 0x0831cb49
    return 1;   // ORIG 0x0831cb4e mov $0x1,%eax
}
// ---- StubS3A_deps 批 ----
// ORIG T 0x0831d164（报告 break_away_prevent__CBreakAwayPreventSystem/
// giveBreakAwayDungeonClearReward.md 全文转写）：order==1 → selectReward(1,
// accCera, user)；否则 selectReward(3, accCera, user)；selectReward 命中
// （返回非 0）则 Inter_LoadEtc::giveBreakAwayReward(user)。
void CBreakAwayPreventSystem::giveBreakAwayDungeonClearReward(CUser* user)
{
    // b28d-D66 终态 50=50 AE（ndiff 37→0，仅重定位显示差）：四处修正
    // ①P0/P198①：删 order/accCera 具名 int 局部 —— ORIG `call getBreakAwayRewardOrder;
    //   cmp $0x1,%eax; sete %al; test %al,%al; je` 直用 %eax；accCera 内联为 selectReward
    //   第 2 实参（`call getBreakAwayAccureCera; mov 0xc(%ebp),%edx; mov %edx,0xc(%esp);
    //   mov %eax,0x8(%esp)`）。
    // ②P198①：`movb $0x0,-0x9` 函数头预清 + 两臂各自 `mov %al,-0x9; cmpb $0x0,-0x9; je`
    //   ⇒ 具名 ok 两句式（声明预清 + 分支赋值）。
    // ③ok 声明 `unsigned char`（非 bool）：bool 局部 `if (!ok)` 产 X 族 `movzbl; xor $1;
    //   test; jne`（54 vs 50），uchar 才复现 ORIG 的 `cmpb $0x0,-0x9; je` 直判。
    // ④P223：ORIG 两臂各有一个空 then 占位岛（0831d201 nop;jmp / 0831d204 nop）⇒
    //   否定 goto + 尾空标签 `if (ok == 0) goto LskipN; give(); LskipN:;` 复现。
    unsigned char ok = 0;
    if (user->getBreakAwayRewardOrder() == 1)
    {
        ok = selectReward(1, user->getBreakAwayAccureCera(), user);
        // P223：ORIG 两臂各有一个 `je <nop 岛>` 中继（0831d201 nop;jmp / 0831d204 nop）
        // ⇒ 用否定 goto + 尾空标签复现。
        if (ok == 0) goto Lskip1;
        Inter_LoadEtc::giveBreakAwayReward(user);   // ORIG 0x084d8e68
    Lskip1:
        ;
    }
    else
    {
        ok = selectReward(3, user->getBreakAwayAccureCera(), user);
        if (ok == 0) goto Lskip2;
        Inter_LoadEtc::giveBreakAwayReward(user);
    Lskip2:
        ;
    }
}

}  // namespace break_away_prevent
