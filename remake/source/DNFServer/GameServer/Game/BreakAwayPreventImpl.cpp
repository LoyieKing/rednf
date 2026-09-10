// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BreakAwayPreventImpl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "BreakAwayPreventSystem.h"
#include "CUser.h"

#include "SigTypes.h"
#include "GlobalData.h"
#include "DB_UpdateGiftCount.h"      // DB_UpdateBreakAwayUvList 权威声明


char* NumberToString(unsigned int value, int radix);   // ORIG T 0x0810904b

#include "BreakAwayPreventImpl/getPrevScheduleTime.inc"
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
#include "BreakAwayPreventImpl/getDiffConnectDay.inc"
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
#include "BreakAwayPreventImpl/calBreakAwayUserPoint.inc"
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
#include "BreakAwayPreventImpl/selectReward.inc"
{
    // ORIG 0x0831c872：map/_List 迭代器均为非 const（_Rb_tree_iterator/_List_iterator）。
    std::map<STRewardDataKey, STRewardDataList>::iterator it =
        m_rewardMap.find(m_keys[keyIndex]);
    if (it == m_rewardMap.end())
        return false;
    // ORIG -0x18 具名 list 指针（0x0831c8f1 operator-> + add $4 落槽，P0 有槽）。
    std::list<STRewardData>* pList = &it->second.m_list;
    // ORIG -0x14 具名 point 副本（0x0831c8f7 mov 0x10(%ebp) → 槽，后续反复重读）。
    int rewardPoint = point;
    if (rewardPoint < 0)
        return false;

    STBreakAwayRewards rewards;   // ORIG STBreakAwayRewards ctor 清零
    // ORIG -0x28 死局部：keyIndex 写后未读（0x0831c918 mov 0xc(%ebp) → 槽）。
    // [M3S5 2026-09-11 · P339 提头试形⇒否] 把 keyIndexKeep 提为**函数头第一个声明**
    //   （期望按「头内先声明更深」落到 ORIG 的 -0x28）⇒ 实测 selectReward
    //   NEAR(143)→**DIFF**（TU 四数 near 1→0、diff 2→3）⇒ 已回滚。残余：ORIG 槽序
    //   li(-0x3c)/rewards(-0x38)/keyIndex(-0x28)/it(-0x24)….pList(-0x18)/point(-0x14)，
    //   我方对应各深 4~7（keyIndex 反向浅 0x14 但提头无效）⇒ 判 N82/N95 槽阶梯封顶。
    int keyIndexKeep = keyIndex;
    (void)keyIndexKeep;
    for (std::list<STRewardData>::iterator li = pList->begin();
         li != pList->end(); ++li)
    {
        // ORIG 0x0831c938：每轮 operator-> 逐字段读 5 次（P157，无 entry 引用缓存）；
        // 读序 field0→field4→field8→fieldc→field10→point。
        const int minVal = li->m_field0;
        const int maxVal = li->m_field4;
        rewards.m_rewardItemId   = li->m_field8;
        rewards.m_rewardAItemCnt = li->m_fieldc;
        rewards.m_luckPoint      = li->m_field10;
        rewards.m_fieldc         = rewardPoint;
        // ORIG 0x0831c992：先 `cmp minVal,p; jle 继续` + 失败直落 return 0（早退形）。
        if (minVal > rewardPoint)
            return false;
        if (minVal <= rewardPoint && maxVal >= rewardPoint)
        {
            rewards.m_rewardItemId   = li->m_field8;
            rewards.m_rewardAItemCnt = li->m_fieldc;
            rewards.m_luckPoint      = li->m_field10;
            rewards.m_fieldc         = rewardPoint;
            user->setBreakAwayRewards(rewards);
            return true;
        }
    }
    user->setBreakAwayRewards(rewards);
    return true;
}

// ---- updateUvList（ORIG 0x0831cb56）----------------------------------------
#include "BreakAwayPreventImpl/updateUvList.inc"
{
    // [M3S5 2026-09-11 · N104 EH pad 封顶 + §6.5 串核对通过]
    //   ORIG 204 / 我方 201（我方少 3 ⇒ 方向=找 ORIG 独有段）。逐段对齐后差异
    //   全在 landing pad 结构：ORIG 有 4 个 pad（0x0831cca6 / 0x0831ccc8 /
    //   0x0831ccdd / 0x0831ce08），其中 0x0831ccdd 是「-0x15 allocator dtor +
    //   _Unwind_Resume」合并块，被前两个 pad 以 `jmp` 复用；我方 3 个 pad、
    //   Unwind 内联在各 pad 尾 ⇒ 少 3 条（4.1 pad 不复用的布局产物，源码不可控）。
    //   语义轴已核（§6.5/.1）：logFormat 实参 = (1, "BreakAwayPreventSystem.cpp",
    //   __PRETTY_FUNCTION__, 0x11c, "BREAK_AWAY : UV_INFO SIZE ERROR!! M_ID(%s)",
    //   NumberToString(login->m_accId, 0)) 与 ORIG 0x0831cd5b..0x0831cd9f 逐项一致；
    //   .rodata 字节比对命中（M_ID 串 @0xbdc300、prett 串 @0xbdcf60 与我方 .o
    //   +0x000/+0x240 逐字节相同；本 TU 无 EUC-KR 字面量，enc_kr_scan 0 命中）
    //   ⇒ 无语义缺口，判 N。
    int days =
        static_cast<int>(getDiffConnectDay(login->m_breakAwayLastUpdateTime));
    if (days < 0)
        days = 0;

    int lastPlayDays =
        static_cast<int>(getDiffConnectDay(login->m_lastPlayTime));

    if (days == 0)
    {
        // ORIG 0x0831cbb2 `test/jne` 直落 lastPlayDays 块、镜像块后置（+0x75）
        // ⇒ 原码 then/else 与直觉相反：then=当日计算、else=镜像返回。
        if (login->m_breakAwayLastUpdateTime == 0)
        {
            // ORIG 0x0831cbb4：if/else 两分支（非三目）。
            if (lastPlayDays == 0)
                days = 1;
            else
                days = lastPlayDays;
        }
        else
        {
            // last_update_time 非 0 且当日：直接镜像登录包 play_info，不落 DB 请求。
            memset(m_tail, 0, sizeof(m_tail));
            memcpy(m_tail, login->m_breakAwayPlayInfo, 0x1f);
            return true;
        }
    }

    // ORIG 0x0831cc1a：days>0x1d 走**前放置的 else 体**（0x0831cc20），string 组
    // 装为 jle 目标的后置 then（0x0831cc59）；makeRequest 统一在尾部（0831ce32），
    // 失败路径 esi=0 经析构后返回 0。
    if (days > 0x1d)
    {
        // 断线 ≥30 天：名单全 '0'（0x30），尾位标记 '1'。
        memset(m_tail, 0x30, sizeof(m_tail));
        m_tail[0x1d] = 0x31;   // this+0x328：名单第 29 位（高位区间标记）
        m_tail[0x1e] = '\0';   // this+0x329
    }
    else
    {
        std::string uvInfo(reinterpret_cast<const char*>(login->m_breakAwayPlayInfo));
        std::string uvMarked(uvInfo, days, 0x1e);
        for (int i = 0; i < days - 1; ++i)
            uvMarked.push_back('0');
        uvMarked.push_back('1');

        if (uvMarked.size() != 0x1e)
        {
            LogManager::logFormat(
                1, "BreakAwayPreventSystem.cpp",
                __PRETTY_FUNCTION__,
                0x11c, "BREAK_AWAY : UV_INFO SIZE ERROR!! M_ID(%s)",
                NumberToString(login->m_accId, 0));
            return false;
        }
        memset(m_tail, 0, sizeof(m_tail));
        strncpy(m_tail, uvMarked.c_str(), uvMarked.size());
    }

    DB_UpdateBreakAwayUvList::makeRequest(login->m_accId, m_tail);
    return true;
}

// ---- startBreakAwayPreventSystem（ORIG 0x0831ce78）-------------------------
#include "BreakAwayPreventImpl/startBreakAwayPreventSystem.inc"
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

    // A 窗口：无归还记录或回归天数越过上限 → luck_point 主通道。
    if (login->m_breakAwayLastRewardTime == 0 || dayDiff >= dayLimit)
    {
        bool pointOk = calBreakAwayUserPoint();
        if (pointOk)
        {   // ORIG 内联（applyWindowReward key=0 order=1 writeOrder）
            if (login->m_breakAwayAccureCera >= 0)
            {
                // ORIG 0x0831cee2：bool 预清 + 赋值两句式（P198①）。
                bool ok = false;
                ok = selectReward(0, login->m_breakAwayAccureCera, user);
                if (ok)
                {
                    user->setBreakAwayRewardData(true);
                    user->setBreakAwayRewardOrder(
                        static_cast<ENUM_BREAK_AWAY_REWARD_ORDER>(1));
                }
            }
            else
            {
                if (login->m_breakAwayLuckPoint == 0)
                    user->setBreakAwayRewardData(false);
            }
        }
        return;
    }

    // 区间 1：second < first（u32 比较）且 dayDiff ∈ [winA_low, win_a_high] → order 3。
    // ORIG 0x0831cf6e：三条件独立短路（无 questFieldsIdle/firstReward 具名局部）。
    if (login->m_breakAwayDungeonClearCnt == 0 &&
        login->m_breakAwayQuestTime != 0 &&
        login->m_breakAwayRewardBoundA > login->m_breakAwayRewardBoundB)
    {
        // ORIG 0x0831cfba/0x0831cfd0：日界两守卫各落**独立** nop 中继岛
        // （0831d158/0831d15b），非共享尾 —— 必须写成两条独立否定 goto
        // （P223 双守卫版；合并进外层 && 链会被 4.4 jump-thread，少 4 条）。
        if ((signed char)(unsigned char)m_keys[2].m_data[0] > dayDiff)
            goto WIN1_LOW_FAIL;
        if (dayDiff > (signed char)(unsigned char)m_keys[2].m_data[1])
            goto WIN1_HIGH_FAIL;
        if (login->m_breakAwayAccureCera >= 0)
        {   // ORIG 内联（applyWindowReward key=2 order=3 writeOrder）
            bool ok = false;
            ok = selectReward(2, login->m_breakAwayAccureCera, user);
            if (ok)
            {
                user->setBreakAwayRewardData(true);
                user->setBreakAwayRewardOrder(
                    static_cast<ENUM_BREAK_AWAY_REWARD_ORDER>(3));
            }
        }
        else
        {
            if (login->m_breakAwayLuckPoint == 0)
                user->setBreakAwayRewardData(false);
        }
        return;
    }

    // 区间 2：first < second 且 dayDiff ∈ [winB_low, win_b_high] → 只置 reward 标志。
    if (login->m_breakAwayDungeonClearCnt == 0 &&
        login->m_breakAwayQuestTime != 0 &&
        login->m_breakAwayRewardBoundA < login->m_breakAwayRewardBoundB &&
        (signed char)(unsigned char)m_keys[4].m_data[0] <= dayDiff &&
        dayDiff <= (signed char)(unsigned char)m_keys[4].m_data[1])
    {
        if (login->m_breakAwayAccureCera >= 0)
        {   // ORIG 内联（applyWindowReward key=4 无 order 写）
            bool ok = false;
            ok = selectReward(4, login->m_breakAwayAccureCera, user);
            if (ok)
            {
                user->setBreakAwayRewardData(true);
            }
        }
        else
        {
            if (login->m_breakAwayLuckPoint == 0)
                user->setBreakAwayRewardData(false);
        }
    }
WIN1_LOW_FAIL: ;
WIN1_HIGH_FAIL: ;
}

} // namespace break_away_prevent

// ---- DB_UpdateBreakAwayUvList::makeRequest（ORIG 0x0843873a；报告缺失，objdump
//      逐指令转写）------------------------------------------------------------
// [b28c-31] AE：Acquire 结果实参内联复现 ORIG 0x0843874x 寄存器直传（70=70）。
#include "BreakAwayPreventImpl/makeRequest.inc"
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

#include "BreakAwayPreventImpl/makeRequest_1.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localchina/localchina_DBThread.cpp", 0x408), true);
    guard.operator*()->operator<<(0x2dd);   // ORIG opcode（localchina DBThread 分发表）
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);   // ORIG operator<<(unsigned)
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

#include "BreakAwayPreventImpl/makeRequest_2.inc"
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

#include "BreakAwayPreventImpl/GM_UpdateUvList.inc"
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
#include "BreakAwayPreventImpl/giveBreakAwayDungeonClearReward.inc"
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
