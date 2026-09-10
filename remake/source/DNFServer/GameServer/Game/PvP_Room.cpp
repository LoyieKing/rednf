#include "LogManager.h"
// ============================================================================
// df_game_r PvP_Room（G2-3 PvP 链，PvP 对战房间聚合根，池化 600）还原
// （2026-08-17）。
// 逐函数对照 docs/class_func_reports/PvP_Room.md（92 节）与 ORIG 反汇编实现；
// 布局依据 PvP_Room.h（0x6e8），子对象走 PvP_deps.h/PvP_deps.cpp。
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径：分支/调用目标→<T>、
// 大地址→<A>、去 @plt、剥尾部 nop）。
// 跨类缺失方法（CUser/GameWorld/统计/脚本等）以 asm-label extern 发调用，
// 链接桩由主 agent / 后续批次提供（PvP_deps.cpp 已定义其中大部分）。
// ============================================================================

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "PvP_Room.h"
#include "CSystemTime.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "TimerQueue.h"
#include "GameWorld.h"
#include "CDataManager.h"
#include "CGameManager.h"        // G_CGameManager / PutPvp / CheckOutPvp
#include "GlobalData.h"          // GlobalData::s_event_manager / s_statistic_proxy_mgr / s_ref_pvp_grade / s_systemTime_
#include "CGuildServerProxy.h"   // CEventManager::GetRepeatEvent
#include "CFairPvPScore.h"
#include "RefPvpGrade.h"
#include "CMissionList_Charac.h"
#include "CPvPLiveEventData.h"
#include "CStatisticServerProxy.h"
#include "CServerProxyMgr.h"
#include "CCommonStatisticsMgr.h"
#include "CDisconnectDetecter.h"
#include "Statistics.h"
#include "CUserProc.h"
#include "Packet_Server_Match_data.h"
#include "MatchRoomList.h"     // IMatch（m_matching 虚表 36 槽权威声明）

// ============================================================================
// ============================================================================
// CSwitchLog（PvP 日志；_ZN10CSwitchLogC1EPKciii / clEPKcz 由 GameStubs 提供）
// ============================================================================
class CSwitchLog
{
public:
    CSwitchLog(const char* name, int line, int a, int b);
    void operator()(const char* fmt, ...);

private:
    char m_pad[0x10];
};
// ============================================================================
// PvP 全局数据（ORIG 定义）：
//   PVP_NORMAL_BATTLE_TIME 0x93f4ee0 / PVP_DM_BATTLE_TIME 0x93f4ee8，各 [2] int
//   —— ORIG 中与 g_equipmentPartSetMap_/characterEquipmentTypeTags 同 TU，
//      定义归 InitScriptProducers_2.cpp（首个具名对象，键名 _GLOBAL__I_PVP_NORMAL_BATTLE_TIME）。
//   PVPMAP_DEATHMATCH_MODE 0x94f70e0 / PVPMAP_ONLY_NORMAL_EXPOSURE 0x94f7108
//   PVPMAP_ONLY_TOURNAMENT_RANDOM 0x94f711c，各 0x14 char（零初始化 BSS）
// ============================================================================
extern int PVP_NORMAL_BATTLE_TIME[2];
extern int PVP_DM_BATTLE_TIME[2];
// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 首个数据定义之前，使 `_GLOBAL__I_`
// 名等于 ORIG 的 `_Z15IsDeathMatchMapiRb`（P327）。数组改用 extern 前置声明，
// 定义仍在下方 ⇒ 符号与初始化语义零变化。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
extern bool PVPMAP_DEATHMATCH_MODE[0x14];
bool IsDeathMatchMap(int mapNo, bool& onlyDeathMatch)
{
    PvPMapInfo info;
    if (CMapList::GetPvpMapInfo(mapNo, info))
    {
        onlyDeathMatch = info.m_playable;
        if (mapNo > 0x13 || mapNo < 0)
            return 0;
        return PVPMAP_DEATHMATCH_MODE[mapNo];
    }
    return 0;
}

bool PVPMAP_DEATHMATCH_MODE[0x14] = {0};
// [b28d-solo-PvP_Room] P248 档②：ORIG GetCandidateMapIndexList 0x085d88xx 对
// `!arr[i]` 发 `movzbl <A>(%eax),%eax; xor $1,%eax; test %al; je/jne`（取反
// 物化）⇒ 元素类型 bool（char 元素时 4.4 恒 `test; jne` 丢 xor）。宽度/符号
// 尺寸零变化（各 0x14 字节）；写侧 InitScriptProducers_8.cpp 以 char 语义写
// 0/1，bool 表示兼容。
bool PVPMAP_ONLY_NORMAL_EXPOSURE[0x14] = {0};
bool PVPMAP_ONLY_TOURNAMENT_RANDOM[0x14] = {0};
// ============================================================================
// 自由函数
// ============================================================================
// ORIG 0x85d4690：查 PvP map；onlyDeathMatch = info.m_playable；
// mapNo∈[0,0x13] 时返回 PVPMAP_DEATHMATCH_MODE[mapNo]
// 末段 ORIG `movzbl tbl(%eax),%eax` 全字直返（无 setne 归一）⇒ 原返回 int +
// (unsigned char) 提升（mangled 无返回类型，零 ABI 风险，技巧33）。
// [b28d-solo-PvP_Room] P248 档①/P292：ORIG 调用点（ChangePvPMode 0x085d8b8e /
// CheckPvPMapMode 0x085dd98e / DrawRandomMapIndex）均 `call; test %al,%al; je`
// 无 setne ⇒ callee 返回 bool；配合 PVPMAP_DEATHMATCH_MODE 改 bool 数组后本函数
// 体尾仍 `movzbl`（bool→bool 无需归一化），自身保持 identical。
// ORIG 0x855c8f4：按频道类型返回匹配类型（9→2, 10→3, 14→4, 15→5, 其余→0）
int GetMatchingType()
{
    int type = 0;
    switch (G_GameWorld()->GetChannelType())
    {
    case 9:  type = 2; break;
    case 10: type = 3; break;
    case 14: type = 4; break;
    // ORIG 0855c946: case15 赋值后 fall-through 到 merge（无 default 复写；
    // 0 已由初始化 0855c8fa 提供）。
    case 15: type = 5; break;
    }
    return type;
}
struct PSeatsView { CUser* m_arr[24]; };
// [b28d-solo-PvP_Room] P210② 跨 TU 迁定义（Main 指派）：ORIG IsRecv 0x085d5ff0 与
// PvP_Room::C1 0x085d6012 地址相邻 ⇒ 原码同 TU；定义迁入本 TU 且置于 4 个 caller
// （recv_pvp_rank_all / recv_pvp_end_result_all / get_recv_pvp_rank_count /
// get_recv_pvp_end_result_count）之前 ⇒ 4.4 精确继承小帧（ORIG 帧 $0x18 / 槽 -0x4）。
// 形按 ORIG：js/jle 双守卫 + movzbl 直返（bool 视图免 setne 归一）。
// [b28d-solo-PvP_Room] AE 归零（13=13，本轮）：P235/P311 —— PvPTypes.h 权威头把
// m_pad 包进匿名 union 增 bool m_bool[0x8] 视角，改具名成员直访问后 4.4 复现
// ORIG 的 `mov 0xc(%ebp),%eax; mov 0x8(%ebp),%edx; movzbl (%edx,%eax,1),%eax`
// （index 先、base 后）；旧 reinterpret_cast 视图形恒 base 先载（NEAR 2 的真因）。
bool PvpResultRecvFlag::IsRecv(int flag) const
{
    if (flag < 0 || flag > 7)
        return false;
    // [b28d-solo-PvP_Room] P235/P311：改用 PvPTypes.h 权威头新增的具名数组视角
    // m_bool[flag]（union 与 m_pad 同宽 8B，sizeof/偏移零变化）⇒ 完整类型成员
    // 直访问复现 ORIG `mov 0xc(%ebp),%eax; mov 0x8(%ebp),%edx; movzbl (%edx,%eax,1),%eax`
    // （index 先、base 后）；旧 reinterpret_cast 视图形恒 base 先载。
    return m_bool[flag];
}
// ORIG 直用成员 m_users[i]（+0x30）/m_teams[i]（+0x50）：偏移为 4 倍数，
// gcc 折叠为 (i + off/4)*4 scale-index（0x30/4=0xc、0x50/4=0x14）。
#define P_SEATS(i)   (m_users[i])
#define P_TEAMS(i)   (m_teams[i])
#define P_SEATSTATE(i) (m_seatState[i])
#define P_WIN(i)     (m_winCheck[i])
#define P_TEAMWIN(i) (m_teamWin[i])
#define P_USERS(i)   (m_users[i])
// ============================================================================
// PvP_Room 实现
// ============================================================================

// ----------------------------------------------------------------------------
// 构造 / 析构
// ----------------------------------------------------------------------------

// [b28d-solo-PvP_Room] N100 已解（Main 代落 P216）：MatchingSystem.cpp:33 + PvPTypes.h:250
// 给 CMatchingSystem::CMatchingSystem() 加 throw() ⇒ 4.4 判 nothrow 不布 landing，
// EXTRA _ZdlPv 消失（116 vs 130）。本轮再把 fill 实参改 `&m_field22c[0]/&m_field22c[15]`
// ⇒ 复现 ORIG 单条 `lea 0x268(%eax),%ecx`（`m_field22c + 15` 形 4.4 拆 add+lea），
// 116 vs 130 → 116 vs 119。残余 3 条：P211（m_matching int 字段 + call 返回值
// (int) 强转 ⇒ RHS-first，ORIG LHS-first 直存）+ N121 中继 jmp ×2；P167 CLEAR、
// 语义全等。
PvP_Room::PvP_Room()
{
    int local_14 = 0;
    std::fill<int*, int>(&m_field22c[0], &m_field22c[15], local_14);
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        P_SEATS(local_10) = 0;
    }
    m_pMatchingSystem = new CMatchingSystem;
    m_matching = (int)m_pMatchingSystem->GetMatchingSystem(ENUM_PVP_MATCHING_TYPE_0);
    m_field61c = 0;
}

// [b28d-solo-PvP_Room] N104（EH 尾块相对序）：ORIG 0x085d61a6 正常路径析构链
// （m_pMatchingSystem delete → Mutex@+0x5e8 → PvpResultRecvFlag@+0x5bc/@+0x5b4 →
// PvpUserTable → PvP_GuildWar_Log）位于函数中段、unwind 块（_Unwind_Resume + 重复
// 析构链）居中；4.4 把同一段 epilogue 就地内联在正常路径、unwind 压最末 ⇒ 两尾块
// 互换，difflib 把 ~9 行 rotate 放大成 ndiff（81 vs 91）。块内容逐条全等，P167
// CLEAR 13/13（nm 实证 ORIG 同名 D1 均在、无发明符号）⇒ 语义完成，byte 层 N104 封顶。
// [SweepPvPRoom2 P141] ORIG 0x085d61a6 只有一次成员读 + `test %ebx,%ebx; je`
// （编译器为 delete 生成的隐式空指针守卫）；手写 `if (m_pMatchingSystem)` 会
// 重复判空（多 mov+mov+test+je 4 条）⇒ 改无条件 delete 复现 ORIG 守卫形。
PvP_Room::~PvP_Room()
{
    delete m_pMatchingSystem;
}

// ----------------------------------------------------------------------------
// 基础访问器（报告缺失，按 ORIG 直接读取）
// ----------------------------------------------------------------------------

void PvP_Room::SetIDX(int idx)
{
    m_index = idx;
    reset();
}

__attribute__((weak)) int PvP_Room::get_index()
{
    return m_index;
}

__attribute__((weak)) int PvP_Room::get_state()
{
    return m_state;
}

__attribute__((weak)) void PvP_Room::set_state(int state)
{
    m_state = state;
}

__attribute__((weak)) int PvP_Room::get_pvp_battle_mode()
{
    return m_pvpBattleMode;
}

__attribute__((weak)) int PvP_Room::GetManager() const
{
    return (int)m_seats[7];
}

__attribute__((weak)) int PvP_Room::GetManagerSeat() const
{
    return m_managerSeat;
}

__attribute__((weak)) unsigned char PvP_Room::isRematchable() const
{
    return m_field61c;
}

__attribute__((weak)) void PvP_Room::setReamtchable(bool flag)
{
    m_field61c = flag;
}

__attribute__((weak)) int PvP_Room::GetRequestedMapNo()
{
    return (int)m_requestedMapNo;
}

__attribute__((weak)) void PvP_Room::SetRequestedMapNo(short mapNo)
{
    m_requestedMapNo = mapNo;
}

__attribute__((weak)) int PvP_Room::GetTeamCount(int team)
{
    int local_f8 = 0;
    for (int local_fc = 0; local_fc < 8; local_fc = local_fc + 1)
    {
        if (P_TEAMS(local_fc) == team)
        {
            local_f8 = local_f8 + 1;
        }
    }
    return local_f8;
}

__attribute__((weak)) void PvP_Room::lock()
{
}

__attribute__((weak)) void PvP_Room::unlock()
{
}
// CheckTimerKey 0x863bccc，报告缺失，objdump 取证）：计数数组即成员
// m_field22c（+0x22c）；ORIG 寻址折叠为 0xc(%this,(idx+0x88)*4)（0x22c=
// 0xc+0x88*4，gcc scale-index 折叠），索引 (int)msg-0x30 越界区为相邻数组带。
#define P_TIMER_KEY(m)   (m_field22c[(int)(m) - 0x30])

unsigned int PvP_Room::gen_timer_key(TIMER_MESSAGE msg)
{
    return ++P_TIMER_KEY(msg);
}

__attribute__((weak)) int PvP_Room::get_timer_key(TIMER_MESSAGE msg)
{
    return P_TIMER_KEY(msg);
}

__attribute__((weak)) bool PvP_Room::CheckTimerKey(TIMER_MESSAGE msg, int key)
{
    return P_TIMER_KEY(msg) == key;
}
__attribute__((weak)) bool PvP_Room::IsExistPassword()
{
    return m_isExistPassword;
}

__attribute__((weak)) bool PvP_Room::IsEndPvpBattle()
{
    return m_isEndPvpBattle;
}

// [SweepPvPRoom2] P292/P321-T3：ORIG 0x082a4322 体 movzbl;ret（bool 字段直返同形），
// 调用点 0x08298134 call;xor $1;test %al;je ⇒ bool（字段与返回同时 bool 化）。
__attribute__((weak)) bool PvP_Room::IsInsertTimerRecvPvpRank()
{
    return m_isInsertTimerRecvPvpRank;
}

// ----------------------------------------------------------------------------
// 积分计算
// ----------------------------------------------------------------------------

// [b28d-7 N97] ORIG 0x085da086：有符号 /2^k 为 mov/sar/shr/lea 四段，4.4 用 mov/test/cmovs 合并形（N97 源形不可造）⇒ 封顶，P167 CLEAR。
// [b28d-solo-PvP_Room] N97: ORIG 0x085da086 有符号 /2^k 为 mov/sar/shr/lea 四段，4.4 用 cmovs 合并形
int PvP_Room::CalculateVictoryPointOfSinglePlay(int rank, bool win, int score)
{
    int local_10;
    if (win)
    {
        return score + 1;                 // ORIG win 分支直返（不写 local_10）
    }
    else if (rank == 1)
    {
        local_10 = (score + 1) / 2;
    }
    else if (rank == 2)
    {
        local_10 = (score + 1) / 4;
    }
    else if (rank == 3)
    {
        local_10 = (score + 1) / 8;
    }
    else if (rank == 4)
    {
        local_10 = (score + 1) / 16;
    }
    else if (rank == 5)
    {
        local_10 = (score + 1) / 32;
    }
    else if (rank == 6)
    {
        local_10 = (score + 1) / 64;
    }
    else if (rank == 7)
    {
        local_10 = (score + 1) / 128;
    }
    else if (rank == 0)
    {
        local_10 = 1;
    }
    else
    {
        cMyTrace local_20(__PRETTY_FUNCTION__,
                          0xb50, 5);
        local_20("invalid rank %d", rank);
    }
    return local_10;
}

int PvP_Room::CalculateVictoryPointOfTeamPlay(int rank, bool win, int score)
{
    // ORIG 085da1f0/085da203：两条路径均不改写 score 形参槽（0x14(%ebp) 只读），
    // 用 return 表达式而非赋值后统一返回。
    if ((win) && (rank == 0))
    {
        return score + 2;
    }
    else if (!win)
    {
        return 1;
    }
    return score;
}

// [b28d-7 N82] ORIG 0x085da210：帧 $0x64 槽组 -0x28 起，4.4 帧 $0x74 槽组 -0x2c 起（不按 lifetime 复用，N82）；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] N82: ORIG 0x085da210 帧 $0x64 槽组 -0x28 起 vs 4.4 $0x74 槽组 -0x2c 起
// [SweepPvPRoom] gap 260=260 归零（旧判 N82 的 gap 3 实为两处真语义 bug + 两处形态差）：
// ①两处内层循环索引 bug：`rank[local_2c]`/`rank[local_24]` → `rank[local_28]`/`rank[local_1c]`
//   （ORIG 0x085da294/0x085da400 `mov -0x24/-0x18(%ebp),%eax; shl $0x2; add rank; mov;
//   cmp -0x28/-0x20(%ebp),%eax` 实证索引是内层循环变量）；
// ②P0 链式形：local_34 读 local_18、local_14 = local_18 - 1 + 循环尾自减（ORIG
//   0x085da475..0x085da487 三条链式赋值）；
// ③P223/BM8 双岛形：两处 `if (P_SEATS==0) goto L1; if (&&) goto L2; {body} L1:; L2:;`
//   复现 ORIG 0x085da24b/0x085da285 与 0x085da399/0x085da3d3 两独立尾岛（嵌套 if/continue
//   被 4.4 jump-thread 合并成单岛，缺 2 条 jmp）。残余 = N82 槽号整体深 0x10（帧同 $0x64）。
void PvP_Room::CalcuateVictoryPoint(int* rank, int rankCount)   // [b28c-16 SIGFIX] ORIG 串 void
{
    if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
    {
        for (int local_2c = 0; local_2c < 8; local_2c = local_2c + 1)
        {
            // [SweepPvPRoom] P223/BM8 双岛形：ORIG 0x085da24b `je 085da363`（条件1 落
            // nop+jmp 岛）与 0x085da285 `jne 085da366`（条件2 落 nop 岛）为两个独立尾岛，
            // body 尾 0x085da361 `jmp 085da367` 跳过两岛；嵌套 if/continue 形被 4.4
            // jump-thread 合并成单岛（缺 2 条 jmp）。多源 goto 到连续尾标签复现。
            if (P_SEATS(local_2c) == 0)
                goto LAB_NEXT1_2C;
            {
                // [DualMisc] ORIG 0x085da2xx：&& 物化为寄存器临时后 jne 跳过（无 xor 取反），
                // 非具名 bool 局部（后者恒落栈 -0x3d、撑大帧 $0x74 vs ORIG $0x64）。
                if (({ (P_SEATS(local_2c) != 0) && (IsPvpObserver(local_2c) != 0); }))
                {
                    goto LAB_NEXT2_2C;
                }
                {
                    int local_28 = 0;
                    while (local_28 < rankCount)
                    {
                        // [SweepPvPRoom] 真语义 bug：ORIG 0x085da294..0x085da29f 为
                        // `mov -0x24(%ebp),%eax; shl $0x2; add rank基址; mov (%eax),%eax;
                        // cmp -0x28(%ebp),%eax`（= rank[local_28] == local_2c）⇒ 索引是
                        // 内层循环变量 local_28，旧形 `rank[local_2c]` 用外层座位号。
                        if (rank[local_28] == local_2c)
                            break;
                        local_28 = local_28 + 1;
                    }
                    if (local_28 == rankCount)
                    {
                        cMyTrace local_54(__PRETTY_FUNCTION__, 0xb7c, 5);
                        local_54("1 PvP_Room::CalcuateVictoryPoint Error, No Rank User Found i<%d> RankCount<%d>",
                                 local_2c, rankCount);
                        goto LAB_085da583;
                    }
                    int local_34 = local_28;
                    bool local_2d = m_userTable.GetWinTeam() == local_2c;
                    P_SEATS(local_2c)->m_winPointBuffer =
                        CalculateVictoryPointOfSinglePlay(local_34, local_2d, m_fieldBc);
                }
            }
LAB_NEXT1_2C: ;
LAB_NEXT2_2C: ;
        }
    }
    else
    {
        for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
        {
            // [SweepPvPRoom] P223/BM8 双岛形（同单挑分支）：ORIG 0x085da399 `je 085da56c`
            // 与 0x085da3d3 `jne 085da56f` 为两个独立尾岛、body 尾 0x085da56a `jmp 085da570`。
            if (P_SEATS(local_24) == 0)
                goto LAB_NEXT1_24;
            {
                // [DualMisc] ORIG 0x085da4xx：同单挑分支，&& 物化后 jne 跳过。
                if (({ (P_SEATS(local_24) != 0) && (IsPvpObserver(local_24) != 0); }))
                {
                    goto LAB_NEXT2_24;
                }
                {
                    int local_20 = GetTeamIndex(P_SEATS(local_24));
                    int local_1c = 0;
                    while (local_1c < rankCount)
                    {
                        // [SweepPvPRoom] 真语义 bug（同单挑分支）：ORIG
                        // 0x085da400..0x085da40b `mov -0x18(%ebp),%eax; shl $0x2; add;
                        // mov (%eax),%eax; cmp -0x20(%ebp),%eax` ⇒ rank[local_1c] == local_24。
                        if (rank[local_1c] == local_24)
                            break;
                        local_1c = local_1c + 1;
                    }
                    if (local_1c == rankCount)
                    {
                        cMyTrace local_44(__PRETTY_FUNCTION__, 0xb97, 5);
                        local_44("1 PvP_Room::CalcuateVictoryPoint Error, No Rank User Found i<%d> RankCount<%d>",
                                 local_24, rankCount);
                        goto LAB_085da583;
                    }
                    int local_18 = local_1c;
                    // [SweepPvPRoom] P0 链式形：ORIG 0x085da475..0x085da487 为
                    // `-0x14 = -0x18`、`-0x30 = -0x14`、`-0x10 = -0x14 - 1`（三条独立
                    // 读 local_1c 的形多 1 条且槽链不符）⇒ local_34 读 local_18、
                    // local_14 初始化即减 1，循环尾自减（4.1 for 形：init; jmp COND;
                    // BODY; subl; COND @0x085da4fc/0x085da500）。
                    int local_34 = local_18;
                    // [SweepPvPRoom-2] 真 bug（Main 高优先级排查）：原 while 形把自减放
                    // 循环体末尾 ⇒ `continue` 跳过自减（遇观察者座位死循环）；ORIG
                    // 0x085da4f3 `je +0x2ec` / 0x085da4fc `subl $0x1,-0x10` 增量在体后、
                    // COND(0x085da500) 之前（for 形）⇒ continue 恒执行增量。改 for 复现。
                    for (int local_14 = local_18 - 1; -1 < local_14; local_14 = local_14 - 1)
                    {
                        int local_10 = rank[local_14];
                        // [DualMisc] ORIG 0x085da6xx：同前，&& 物化后 jne 跳过（无 xor）。
                        if (({ (P_SEATS(local_10) != 0) && (IsPvpObserver(local_10) != 0); }))
                        {
                            continue;
                        }
                        if (GetTeamIndex(P_SEATS(local_10)) != local_20)
                        {
                            local_34 = local_34 - 1;
                        }
                    }
                    bool local_2d = m_userTable.GetWinTeam() == local_20;
                    P_SEATS(local_24)->m_winPointBuffer =
                        CalculateVictoryPointOfTeamPlay(local_34, local_2d, m_fieldBc / 2);
                }
            }
LAB_NEXT1_24: ;
LAB_NEXT2_24: ;
        }
    }
LAB_085da583:
    ;
}

// ----------------------------------------------------------------------------
// 房间流程
// ----------------------------------------------------------------------------

bool PvP_Room::CheckPassword(const char* password, int len)
{
    if (m_passwordLen != len)
    {
        return 0;
    }
    return memcmp(m_password, password, m_passwordLen) == 0;
}

// [b28d-solo-PvP_Room] P210 定义前置：caller 置于 IsPvpObserver 定义之前 ⇒ 4.4 保守
// outgoing（帧 $0x28、槽基 -0xc），与 ORIG 0x085d65e8 逐条一致；旧 P190 pads 形帧同
// 但槽基浅 8（-0x4）已废。
bool PvP_Room::check_start_state()
{
    lock();
    // [b28d-solo-PvP_Room] ORIG 0x085d6602 前两条件落 bool 物化岛（mov$1/jmp/
    // mov$0/test/je）再与第三条件短路；for(i<=7) setle 尾测（P226+P51）。
    for (int local_10 = 0; local_10 <= 7; local_10 = local_10 + 1)
    {
        if (({ (P_SEATS(local_10) != 0) && (!IsPvpObserver(local_10)); }))
        {
            if (!P_SEATSTATE(local_10))
            {
                unlock();
                return 0;
            }
        }
    }
    unlock();
    return 1;
}

// [b28d-solo-PvP_Room] P210 定义前置：caller 置于 IsPvpObserver 定义之前 ⇒ 4.4 保守
// outgoing（帧 $0x28、槽基 -0xc），与 ORIG 0x085d6774 一致；旧 P190 pads 形只撑帧
// 不改槽基（-0x4 起）已废。P169 外层正向 if 包内层 goto 使三 je 共落 BODY。
int PvP_Room::get_pvp_team_cnt()
{
    int local_18;
    int local_14;
    if (((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3)) || (m_pvpBattleMode == 5))
        goto TEAM_BODY;
    return -1;
TEAM_BODY:
    {
        local_18 = 0;
        local_14 = 0;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            // [b28d-7] P214①：ORIG 正条件 bool 物化（mov$1/jmp/mov$0）+失败跳尾
            // ⇒ 源形=直接 && 包裹（if/else 两臂赋值反产 movb/cmpb）。
            if ((P_SEATS(local_10) != 0) && (!IsPvpObserver(local_10)))
            {
                if (P_TEAMS(local_10) == 1)
                {
                    local_18 = local_18 + 1;
                }
                else if (P_TEAMS(local_10) == 2)
                {
                    local_14 = local_14 + 1;
                }
            }
        }
        if (local_14 != local_18)
        {
            return -1;
        }
    }
    return local_14;
}

int PvP_Room::get_player_count()
{
    int local_14 = 0;
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) != 0 && !IsPvpObserver(local_10))
        {
            local_14 = local_14 + 1;
        }
    }
    return local_14;
}

// [b28d-solo-PvP_Room] P210 定义前置：caller 置于 callee(IsPvpObserver) 定义之前 ⇒ 4.4
// 保守 outgoing（帧 $0x28、槽基 -0x14/-0x10/-0xc），与 ORIG 0x085d7743 逐条一致；
// 旧 P190 双取址 double pads 形只撑帧不改槽基（槽仍 -0xc 起）已废。
int PvP_Room::is_closeable_seat_state()
{
    int local_18 = 0;
    int local_14 = 0;
    for (int local_10 = 0; local_10 <= 7; local_10 = local_10 + 1)
    {
        if ((P_SEATS(local_10) != 0) && (!IsPvpObserver(local_10)))
        {
            local_18 = local_18 + 1;
        }
        if (P_TEAMS(local_10) == 0xff)
        {
            local_14 = local_14 + 1;
        }
    }
    if ((local_18 == 1) && (local_14 < 2))
    {
        return 0;
    }
    return 1;
}

bool PvP_Room::IsPvpObserver(int seat)
{
    if (seat > 7)
    {
        return 0;
    }
    if ((m_teams[seat] == 3) || (m_teams[seat] == 4))
    {
        return 1;
    }
    return 0;
}

int PvP_Room::check_joinable(CUser* user)
{
    // [b28d-7] P16 早返结构：ORIG 0x085dbd3c `je BODY; mov$4; jmp END; BODY:`
    // ⇒ 失败块（return 4）物理居前，if/else 形反产 jne+双 return 4 尾。
    if (m_state != 1)
    {
        return 4;
    }
    int local_10 = 0;
    if ((local_10 = ((IMatch*)m_matching)->CheckJoinable(this, user)) != 0)
    {
        return local_10;
    }
    for (int local_f4 = 0; local_f4 < 8; local_f4 = local_f4 + 1)
    {
        if ((P_SEATS(local_f4) == 0) && (m_teams[local_f4] == 0xff))
        {
            return 0;
        }
    }
    return 4;
}

bool PvP_Room::IsQuickJoinablePVPRoom(CUser* user)
{
    // P24：ORIG 内联实参（0x085d6c1c），getCurCharacNo 结果直传不落栈槽。
    // ORIG 0x085dc18 逐段：layer1 `call check_joinable; test%eax; jne L0(mov$0)`；
    // layer2 `call find…; xor$1; test%al; je L0`；真值路 mov$1，尾 `test%al;
    // je; mov$1/jmp; mov$0` 双归一岛（&&-结果岛 + return 归一岛）。P0：结果全程
    // %eax 无槽 ⇒ 源形 = 短路 && 条件 + if-return 1/0（无具名局部，P56 形）。
    if (check_joinable(user) == 0 &&
        !user->find_pvp_masterid_walkingout_me(m_seats[7]->getCurCharacNo()))
        return 1;
    return 0;
}

int PvP_Room::get_team(int seat)
{
    if ((seat < 0) || (7 < seat))
    {
        return 0xfe;
    }
    return P_TEAMS(seat);
}

int PvP_Room::get_manager_team()
{
    if ((m_managerSeat < 0) || (7 < m_managerSeat))
    {
        return 0xfe;
    }
    return P_TEAMS(m_managerSeat);
}

int PvP_Room::get_user_seat(CUser* user)
{
    for (int local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) == user)
            return local_8;
    }
    return -1;
}

int PvP_Room::get_user_seat(int seat)
{
    // ORIG 085d921e：无帧；`mov $0;jmp pop` 块物理在前、body 尾块 fall 直 ret。
    // 越界或形守卫（js/jle 双入 return0 块）复现块序。
    if ((seat < 0) || (seat > 7))
        return 0;
    return (int)P_SEATS(seat);
}

int PvP_Room::GetTeamIndex(CUser* user)
{
    // ORIG 0x085dc6ee：for+jmp-check+break 岛；-0x4=游标、-0x8=结果。
    int local_8;
    int local_4;
    for (local_4 = 0; local_4 < 8; local_4 = local_4 + 1)
    {
        if (P_SEATS(local_4) == 0)
        {
            continue;
        }
        if (P_SEATS(local_4) == user)
        {
            break;
        }
    }
    if (local_4 == 8)
    {
        return -1;
    }
    if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
    {
        local_8 = local_4;
    }
    else
    {
        local_8 = P_TEAMWIN(local_4);
    }
    return local_8;
}

// [b28d-solo-PvP_Room] P248 档①/P292: 返回 int→bool（ORIG set_ready_state 0x085d8368 调用点 `call; xor $1,%eax; test %al,%al; je`）；mangled 不变、自身 AE
bool PvP_Room::check_team_pvp()
{
    // [b28d-7] ORIG 头部 0x085da6xx：`cmp $5; je BODY; mov$1; jmp END; BODY:`
    // ⇒ 正条件 goto + return 1 fall-through（if 包裹形反产 jne）；
    // 尾部 `je RET1; mov$0; jmp; RET1: mov$1` ⇒ P47 三目形。
    if (((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3)) || (m_pvpBattleMode == 5))
        goto CT_PVP_BODY;
    return 1;
CT_PVP_BODY:
    {
        if (G_GameWorld()->GetChannelType() == 7)
        {
            return 1;
        }
        int local_18 = 0;
        int local_14 = 0;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if ((P_SEATS(local_10) != 0) && (!IsPvpObserver(local_10)))
            {
                if (P_TEAMS(local_10) == 1)
                {
                    local_18 = local_18 + 1;
                }
                else if (P_TEAMS(local_10) == 2)
                {
                    local_14 = local_14 + 1;
                }
            }
        }
        if (local_14 != local_18)
            return 0;
        return 1;
    }
}

int PvP_Room::get_remain_seat() const
{
    return 8 - get_waiter_count();
}

int PvP_Room::get_waiter_count() const
{
    int local_c = 0;
    for (int local_8 = 0; local_8 < 8; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) != 0)
        {
            local_c = local_c + 1;
        }
    }
    return local_c;
}

int PvP_Room::count_pvp_user()
{
    // ORIG 0x085da840：continue 形产生 true 尾 jmp merge + false nop 岛。
    int local_c = 0;
    for (int local_8 = 0; local_8 < 8; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) == 0)
        {
            continue;
        }
        local_c = local_c + 1;
    }
    return local_c;
}

// [b28d-solo-PvP_Room] P292/P248 档①：ORIG 0x085dd070 调用点
// `call check_authority; xor $0x1,%eax; test %al,%al; je`（无 setcc）⇒ callee bool；
// 旧 int 返回产 `test %eax,%eax; sete %al`（+1 条）。返回类型不入 mangled、
// 函数体两形产物相同（ORIG 本体 0x085d65b4 仍 identical）。
bool PvP_Room::check_authority(CUser* user, int seat)
{
    if (m_seats[7] == user)
    {
        return true;
    }
    if (P_SEATS(seat) == user)
    {
        return true;
    }
    return false;
}

// [b28d-7 N95] ORIG 0x085dc2cc 槽 -0x8/-0x4（帧 $0x18）vs 4.4 -0x10/-0xc（帧 $0x28）：N95 槽基 gap + 帧量化，P167 CLEAR。
// [b28d-solo-PvP_Room] NEAR(39=39): P210② IsRecv 定义迁入本 TU ⇒ 帧 $0x18/槽
// -0x8/-0x4 归位；P223 continue 形复现三 je 分落岛（+0x5b/+0x5e/+0x5f 偏移全同）；
// 残余仅 call 目标/跳转目标的重定位显示差（N124 工具口径）。
int PvP_Room::get_recv_pvp_rank_count()
{
    // ORIG 0x085dc2cc：bool IsRecv 直用条件（test %al/je，无 !=0 物化）；
    // 槽 -0x8=count、-0x4=i，帧 $0x18。
    int local_c = 0;
    for (int local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        // [b28d-solo-PvP_Room] P223 continue 形：ORIG 0x085dc2cc 前两守卫落
        // L1(nop;jmp)/L2(nop) 岛、第三守卫（!IsRecv）直落 L3(i++)。
        if (P_SEATS(local_8) == 0)
            continue;
        if (P_TEAMS(local_8) == 0xff)
            continue;
        if (m_recvFlagB4.IsRecv(local_8))
            local_c = local_c + 1;
    }
    return local_c;
}

// [b28d-7 N95] ORIG 0x085dc1c8 槽 -0x8/-0x4（帧 $0x18）vs 4.4 -0x10/-0xc（帧 $0x28）：N95 槽基 gap + 帧量化，P167 CLEAR。
// [b28d-solo-PvP_Room] NEAR(39=39): P210② IsRecv 定义迁入 ⇒ 帧 $0x18/槽 -0x8/-0x4
// 归位；P223 continue 形复现三 je 分落岛；残余仅重定位显示差（N124）。
int PvP_Room::get_recv_pvp_end_result_count() const
{
    int local_c = 0;
    for (int local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        // [b28d-solo-PvP_Room] P223 continue 形（同 get_recv_pvp_rank_count 判例）：
        // ORIG 0x085dc1c8 前两守卫落 L1(nop;jmp)/L2(nop) 岛、第三守卫直落 i++；
        // 正极性 `if (IsRecv) count++` 即产三 je 分落 + count++ 后 jmp 全等形。
        if (P_SEATS(local_8) == 0)
            continue;
        if (P_TEAMS(local_8) == 0xff)
            continue;
        if (m_recvFlagBC.IsRecv(local_8))
            local_c = local_c + 1;
    }
    return local_c;
}

// [b28d-7 N95] ORIG 0x085dc4ba 槽基 -0x4（帧 $0x18）vs 4.4 -0xc（帧 $0x28）：4.4 局部槽基恒留 8B gap 不可上移（N95 判例），指令/语义全等、P167 CLEAR。
// [b28d-solo-PvP_Room] AE: P210② IsRecv 定义迁入本 TU 并前置（Main 指派）⇒ 4.4
// 精确继承小帧，ORIG 0x085dc4ba 帧 $0x18/槽 -0x4 归位；原 N95 槽基差随定义可见性消除。
int PvP_Room::recv_pvp_rank_all() const
{
    for (int local_8 = 0; local_8 <= 7; ++local_8)
    {
        if (P_SEATS(local_8) == 0)
            continue;
        if (P_TEAMS(local_8) == 0xff)
            continue;
        if (m_recvFlagB4.IsRecv(local_8) == false)
            return 0;
    }
    return 1;
}

// [b28d-7 N95] ORIG 0x085dc07e 槽基 -0x4（帧 $0x18）vs 4.4 -0xc（帧 $0x28）：N95 槽基 gap 封顶，P167 CLEAR。
// [b28d-solo-PvP_Room] AE: P210② IsRecv 定义迁入 ⇒ ORIG 0x085dc07e 帧 $0x18/槽 -0x4 归位。
int PvP_Room::recv_pvp_end_result_all() const
{
    for (int local_8 = 0; local_8 <= 7; ++local_8)
    {
        if (P_SEATS(local_8) == 0)
            continue;
        if (P_TEAMS(local_8) == 0xff)
            continue;
        if (m_recvFlagBC.IsRecv(local_8) == false)
            return 0;
    }
    return 1;
}

// [b28d-7 N95] ORIG 0x085dc0f0：槽 -0x4/-0x8（帧 $0x18）vs 4.4 -0xc/-0x10（帧 $0x28）+ do-while 旋转点差；P167 CLEAR，N95 封顶。
// [b28d-solo-PvP_Room] NEAR(70=70): P210② IsRecv 定义迁入 ⇒ 帧 $0x18/槽 -0x4/-0x8
// 归位；第一循环合并守卫（je/jne 共落 L4f）、第二循环 continue 双岛（P223）复现
// ORIG 0x085dc0f0 结构；残余仅重定位/寄存器角色显示差。
int PvP_Room::recv_pvp_end_result_team(CUser* user) const
{
    // [b28d-solo-PvP_Room] ORIG 0x085dc0f0：i 双初始化（movl $0,-0x8 两次）、
    // 两段 for(i<=7) setle 尾测、team 存 -0x4；4.4 do-while+goto 形全帧漂移。
    int local_8 = 0;
    int local_4 = 0xff;
    for (local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        // [b28d-solo-PvP_Room] ORIG 0x085dc11b/0x085dc12c 两守卫 je/jne 共落
        // L4f（无岛）⇒ 合并短路条件形（两个独立 continue 会产 nop;jmp 岛）。
        if (P_SEATS(local_8) != 0)
        {
            if (P_SEATS(local_8) == user)
            {
                local_4 = P_TEAMS(local_8);
                break;
            }
        }
    }
    if (local_4 == 0xff)
    {
        return 0;
    }
    for (local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) == 0)
            continue;
        if (P_TEAMS(local_8) != local_4)
            continue;
        if (m_recvFlagBC.IsRecv(local_8) != 1)
        {
            return 0;
        }
    }
    return 1;
}

bool PvP_Room::set_recv_pvp_rank_flag(CUser* user)
{
    int iVar2 = get_user_seat(user);
    if (iVar2 < 0)
    {
        return 0;
    }
    if (m_recvFlagB4.IsRecv(iVar2))
    {
        return 0;
    }
    m_recvFlagB4.SetRecvFlag(iVar2);
    return 1;
}

int PvP_Room::set_recv_pvp_end_result_flag(CUser* user)
{
    // ORIG 0x085dbfec：seat<0 早退 mov $0（fall 侧），显式 if/return 物化，
    // 非 bool 算术化（避免 not/shr 折叠）。
    int seat = get_user_seat(user);
    if (seat < 0)
    {
        return 0;
    }
    m_recvFlagBC.SetRecvFlag(seat);
    return 1;
}
bool PvP_Room::is_last_win(int seat)
{
    if (P_SEATS(seat) == 0)
    {
        return false;
    }
    int iVar1 = GetTeamIndex(P_SEATS(seat));
    return m_userTable.GetWinTeam() == iVar1;
}

// [b28d-solo-PvP_Room] P169 修复: ORIG 0x085da58a jg FALSE/jns BODY/单一 FALSE 块；外层正向 if 包内层 goto 复现，已归零
bool PvP_Room::IsAllMoreThanGrade(int grade)
{
    // [b28d-solo-PvP_Room P169] ORIG 0x085da58a：`cmp $0x25; jg FALSE;
    // cmp $0; jns BODY; FALSE: mov$0; jmp END; BODY:`（单一 FALSE 块、return 1
    // fall-through）。P169 形（外层正向 if + 内层 goto GRADE_BODY）逐条复现头部
    // jg/jns 与单 FALSE 块；残余 1 条 nop 落点差由 compare_tu nop 剥离口径吸收。
    if (grade <= 0x25)
    {
        if (grade >= 0)
            goto GRADE_BODY;
    }
    return false;
GRADE_BODY:
    for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
    {
        // [b28d-solo-PvP_Room] ORIG 0x085da5b8 把 seat 存 -0xc 槽（P0 具名
        // 局部），且第二条件发 setl/test/je（P225 嵌套 if 复现）。
        CUser* local_c = P_SEATS(local_14);
        if (local_c != 0)
        {
            if (local_c->get_pvp_grade() < grade)
            {
                return false;
            }
        }
    }
    return true;
}

int PvP_Room::check_acquire_guild_point()
{
    if (G_GameWorld()->IsPVPChannel())
    {
        if (G_GameWorld()->IsFreePvPChannel())
        {
            return 0;
        }
        if (!IsAllMoreThanGrade(10))
        {
            return 0;
        }
        if (IsThereSameGuildMember())
        {
            return 0;
        }
        return 1;
    }
    return 0;
}
// [b28d-7 N82] ORIG 0x085d96ec：大帧 `add $<A>,%esp`（槽组 -0x28..-0x68），4.4 `sub $0x90` + 槽组深移；P167 CLEAR ⇒ N82 槽复用策略封顶。
// [b28d-solo-PvP_Room] N82: ORIG 0x085d96ec 大帧槽组 -0x28..-0x68 vs 4.4 -0x90；P167 CLEAR
bool PvP_Room::check_winner()
{
    // ORIG 0x085d98ea（274 条；本侧 290，+16 全为 N 类形态差，台账豁免）：
    //  - 帧调整 4.1 `add $0xffffff80,%esp`（负常量折叠）vs 4.4 `sub $0x90`；
    //    帧洞 0x10（outgoing frame N）；
    //  - local_6c[P_TEAMWIN+3] 索引 4.1 单 lea 折叠、4.4 拆 mov+add 两段
    //    （N18a/N76）；bVar 常量岛 `mov$1/mov$0/test;je`×3（setcc-fold）；
    //  - mode5 段 &m_deathMatch 先期 lea 缓存（P52 callee-saved），4.4 实参处
    //    现算。GetRanking/insert 调用链与错误码逐段核对无语义缺口。
    // [b28d-t3-PoolBigThree] 求值次数形实测（保持 += 单语句形，insn 290→288）：
    // `local_6c[T(i)+3] += 1` 在 4.4 仍产双求值（`lea 0x3(,%eax)` + 重算 `add $0x3`
    // 两段，ORIG 0x085d9799 单 %eax scaled 复用）⇒ 4.1 -O0 表达式树 CSE 不存在于
    // 4.4，具名局部会引入 ORIG 无的索引槽（反劣）。N18a/N76 判词维持，ndiff 171。
    if (m_fieldC0 != 0)
    {
        return 0;
    }
    if ((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3))
    {
        lock();
        int local_6c[6];
        local_6c[3] = 0;
        local_6c[4] = 0;
        local_6c[5] = 0;
        local_6c[0] = 0;
        local_6c[1] = 0;
        local_6c[2] = 0;
        for (int local_2c = 0; local_2c < 8; local_2c = local_2c + 1)
        {
            if ((P_TEAMWIN(local_2c) != 0) && (P_TEAMWIN(local_2c) < 0xfe))
            {
                // [b28d-t3-PoolBigThree] 求值次数形：`a[x]=a[x]+1` 双侧下标 = 二次求值
                // （4.4 输出双段 lea 0x3 + add 0x3 共 12 条）；ORIG 0x085d9799 单次
                // 求值后 `mov -0x5c(,%eax,4); add; mov` 6 条 ⇒ 改 += 单求值形。
                local_6c[P_TEAMWIN(local_2c) + 3] += 1;
                if (P_WIN(local_2c) != 0)
                {
                    local_6c[P_TEAMWIN(local_2c)] += 1;
                }
            }
        }
        int local_34 = 0;
        unsigned int local_30 = 0;
        for (int local_28 = 1; (int)local_28 < 3; local_28 = local_28 + 1)
        {
            if (0 < local_6c[local_28])
            {
                local_34 = local_34 + 1;
                local_30 = local_28;
            }
        }
        if (local_34 == 1)
        {
            for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
            {
                if (P_TEAMWIN(local_24) == local_30)
                {
                    P_WIN(local_24) = 1;
                }
                else
                {
                    P_WIN(local_24) = 0;
                }
            }
        }
        unlock();
        if (local_34 == 1)
        {
            m_userTable.SetWinTeam(local_30);
            return 1;
        }
    }
    else if (m_pvpBattleMode == 4)
    {
        int local_20 = 0;
        for (int local_1c = 0; local_1c < 8; local_1c = local_1c + 1)
        {
            if ((P_SEATS(local_1c) != 0) && (P_TEAMS(local_1c) != 4))
            {
                local_20 = local_20 + 1;
            }
        }
        cMyTrace local_54(__PRETTY_FUNCTION__, 0xa3c, 0);
        local_54("PvP_Room::check_winner single : %d", local_20);
        if (m_deathMatch.checkGameOverForSinglePlay(local_20) != 0)
        {
            m_deathMatch.checkWinnerForSinglePlay(m_users, m_winCheck, m_userTable);
            return 1;
        }
    }
    else if (m_pvpBattleMode == 5)
    {
        int uVar2 = get_pvp_team_cnt();
        cMyTrace local_44(__PRETTY_FUNCTION__, 0xa46, 0);
        local_44("PvP_Room::check_winner team  : %d", uVar2);
        int iVar3 = get_pvp_team_cnt();
        if (m_deathMatch.checkGameOverForTeamPlay(m_users, iVar3, m_teamWin) != 0)
        {
            m_deathMatch.checkWinnerForTeamPlay(m_users, m_winCheck, m_teamWin, m_userTable);
            return 1;
        }
    }
    else
    {
        lock();
        int local_18 = 0;
        int local_14 = 0;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if ((P_WIN(local_10) != 0) && (P_TEAMS(local_10) != 4))
            {
                local_18 = local_18 + 1;
                local_14 = local_10;
            }
        }
        unlock();
        if (local_18 == 1)
        {
            m_userTable.SetWinTeam(local_14);
            return 1;
        }
    }
    return 0;
}

// ----------------------------------------------------------------------------
// 房间生命周期
// ----------------------------------------------------------------------------

// [b28d-7 N82] ORIG 0x085d6908：形参 spill 槽 -0x3c/-0x40/-0x44/-0x48，4.4 -0x4c/-0x50/-0x54/-0x24（帧 0x5c vs 0x6c）；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] 本轮（207→198，ORIG 195）：真语义 bug 修复 —— RequestMap 实参
// 应为栈上 short 形参 b + 形参 mode（ORIG `lea -0x40(%ebp)`/`lea 0x28(%ebp)`），旧形传
// 成员 m_requestedMapNo/m_pvpBattleMode；P77/P0 形参 a/b/c 直用（消 4.4 双层复制 6 条）；
// P0 strlen 内联。残余 P211（m_matching int 字段 + (int)void* 异型 store 序）+ N82（local_48 槽）。
int PvP_Room::create_room(CUser* user, char a, char* name, short b, bool c,
                          const char* password, int d, PVP_BATTLE_MODE mode)
{
    // [b28d-solo-PvP_Room] P77/P0：ORIG 0x085d6908 的 -0x3c/-0x40/-0x44/-0x48 是形参
    // a/b/c/mode 的 home slot（4.1 形参 spill），后续一律从 spill 槽读（m_mode8←-0x3c、
    // m_requestedMapNo←-0x40、m_isExistPassword←-0x44）⇒ 原码**无具名局部**，直接用形参
    // （旧形多一层局部复制，+6 条）。
    // mode 例外：ORIG 在 a/b/c spill 之后立即 `mov 0x28(%ebp),%eax; mov %eax,-0x48`
    // ⇒ mode 有具名 int 局部（日志/m_pvpBattleMode 均经 -0x48 读）。
    int local_48 = (int)mode;
    // ORIG +0x25..+0x41：三值以 callee-saved（%edi/%esi/%ebx）跨后续调用缓存
    // （P87 register 形），日志首参为 __PRETTY_FUNCTION__ 局部静态（P118/P139）。
    register int roomIdx = m_index;
    register const char* characName = user->getCurCharacName();
    register const char* accName = user->get_acc_name();
    {
        CSwitchLog(__PRETTY_FUNCTION__, 0x43a, 0, 0)(
            "pvp@log %s,%s,room(%d),%d", accName, characName, roomIdx, local_48);
    }
    if (user->getCurCharacR() == 0)
    {
        return 0x13;
    }
    m_matching = (int)m_pMatchingSystem->GetMatchingSystem((ENUM_PVP_MATCHING_TYPE)GetMatchingType());
    // [b28d-solo-PvP_Room] 真语义 bug（实参取错对象）：ORIG 0x085d6908 调用点
    // `lea -0x40(%ebp),%edx; mov %edx,0x4(%esp)`（=&local_44 短整局部）+
    // `lea 0x28(%ebp),%edx; mov %edx,0x8(%esp)`（=&mode 第 8 形参）⇒ 原码传
    // 栈上局部/形参引用，非成员 `m_requestedMapNo`/`m_pvpBattleMode`（旧形传
    // this+0x5c4/this+4，被调侧读的是成员地址，语义与 ORIG 不同）。
    ((IMatch*)m_matching)->RequestMap(b,
                                      reinterpret_cast<PVP_BATTLE_MODE&>(mode));
    if (m_state != 0)
    {
        return 0x13;
    }
    m_state = 1;
    m_seats[7] = user;
    m_managerSeat = 0;
    P_USERS(0) = user;
    P_TEAMS(0) = 1;
    m_relayMgr.OnChangeTeam(0, 1);
    m_mode8 = a;
    if (m_mode8 == 0)
    {
        // [b28d-solo-PvP_Room] P0：ORIG 0x085d6908 `call strlen; mov %eax,0x8(%esp)`
        // （结果无槽往返直压实参）⇒ 原码是内联 `strlen(name)`，无具名 len 局部
        // （旧形多 mov %eax,-0x1c / 回读 两条）。
        strncpy(m_roomName, name, strlen(name));
    }
    user->SetPvpIndex((short)m_index);
    m_joinCount = 1;
    m_requestedMapNo = b;
    m_playMapNo = m_requestedMapNo;
    m_isExistPassword = c ? 1 : 0;
    if (m_isExistPassword != 0)
    {
        m_passwordLen = d;
        memset(m_password, 0, 9);
        memcpy(m_password, password, m_passwordLen);
    }
    m_relayMgr.OnCreateRoom(m_managerSeat);
    m_deathMatch.OnCreateRoom(m_managerSeat);
    // ORIG `mov 0x28(%ebp),%edx`（形参槽）⇒ 此处用形参 mode，local_48 仅日志用。
    m_pvpBattleMode = mode;
    int ret = 0;
    if ((ret = ((IMatch*)m_matching)->OnCreateRoom(user)) != 0)
    {
        return ret;
    }
    return 0;
}

// §6.2 巨件核销：P167 102/102 [0缺0多]；gap 702→+7；残差归因 N82（ORIG 帧
// 0x9c/21 槽 vs 4.4 帧 0xac/23 槽，PacketGuard/CSwitchLog 对象槽平移）+ 4.4
// 布尔物化（sete/cmpb/movb/jcc+1）；逐块人工比对语义与 Ghidra C 一致（含
// OnJoinRoom(seat) 实参、m_state==2 判 0x13、双 PacketGuard 布局；详见
// docs/asm_translation_patterns.md 负例表）⇒ N 封顶。
// [b28d-7 N82] ORIG 0x085d6cf2：帧 $0x9c + 形参 spill 槽 -0x7c 起，4.4 帧 $0xac + 槽 -0x8c 起（不按 lifetime 复用）；702/708、P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] 本轮（708→706）：`flag ? 1 : 0`→`(int)flag`（ORIG movzbl 直扩）。
// [b28d-t3-PoolBigThree] **679→316**：bVar3 三段 if/else-if+槽读回 改 `if (a==0xf ||
// a==0x10)` 直判（ORIG 0x085d6dae..6de0 双调用+值岛 %eax 实证，见函数体判词）。
// 残余 316 = N82 帧 0x9c/0xac + 槽号整体平移（-0x6c↔-0x74 族）+ N7（ORIG iVar9→%ebx/
// iVar10→%esi callee-saved 缓存 cmp+setl/setle 物化，4.4 槽往返+jge/jg 直判，
// @0x085d6df1..）+ PacketGuard ctor 发射时刻。p167 102/102 CLEAR。
// [SweepPvPRoom] 复核 gap=2（700 vs 702）：多重集差为 ORIG 的 %edi/%esi/%ebx
// callee-saved 缓存（`mov %eax,%edi/%esi/%ebx` + 对应 push/pop）vs 4.4 槽往返 +
// 帧 $0x9c/$0xac；p167 call 差集空（dual_rank miss=0 extra=0）⇒ N82/N82附 封顶。
int PvP_Room::join_room(CUser* user, int& seat, bool flag)
{
    int local_48 = get_waiter_count();
    int local_7c[3];
    PacketGuard local_64;
    PacketGuard local_70;
    {
        CSwitchLog local_58(__PRETTY_FUNCTION__, 0x4a8, 0, 0);   // ORIG _ZZ…__PRETTY_FUNCTION__ 同文（P139）
        // [b28d-solo-PvP_Room] ORIG 0x085d6cf2：flag 以 `movzbl -0x7c(%ebp),%eax;
        // mov %eax,-0x88(%ebp)` 直扩（bool→int 转换），`flag ? 1 : 0` 会多 mov$1/$0
        // 物化岛 ⇒ 改 (int)flag。
        local_58("pvp@log %s,%s,room(%d),%d,%d,%d", user->get_acc_name(),
                 user->getCurCharacName(), m_index, m_pvpBattleMode, (int)flag, local_48);
    }
    // [b28d-t3-PoolBigThree] ORIG 0x085d6dae..6de0 实测：`call GetChannelType; cmp
    // $0xf; je <true岛>; call GetChannelType; cmp $0x10; jne <false岛>; mov $1,%eax;
    // jmp; mov $0,%eax; test %al; je` = **|| 短路求值直进条件**（两次独立调用、值岛
    // 在 %eax、无 bool 槽）。旧三段 if/else-if 产 `sete;test;je` + `movb $1/$0,-0x4d`
    // 槽往返（两处 +7 条）⇒ 改 if (a==0xf || a==0x10) 直判形。
    if ((G_GameWorld()->GetChannelType() == 0xf) ||
        (G_GameWorld()->GetChannelType() == 0x10))
    {
        int iVar9 = G_CDataManager()->GetMaxGradePvPChannel();
        int iVar10 = user->get_pvp_grade();
        if (iVar9 < iVar10)
        {
            return 0xd;
        }
        int iVar11 = (int)((fair_pvp::CFairPvPScore*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7))->GetGiveItem();
        iVar10 = user->get_pvp_grade();
        iVar11 = G_CDataManager()->m_serverParameter.get_pvp_item_give(iVar10);
        if (iVar9 <= iVar11)
        {
            return 7;
        }
    }
    if (user->getCurCharacR() == 0)
    {
        return 0x13;
    }
    if (m_state == 2)
    {
        return 0x13;
    }
    local_48 = get_waiter_count();
    local_64.put_header(0, 2);
    local_70.put_header(0, 0xb);
    local_64.put_byte(1);
    local_64.put_short(local_48);
    local_70.put_byte(local_48);
    for (int local_3c = 0; local_3c < 8; local_3c = local_3c + 1)
    {
        if (P_SEATS(local_3c) != 0)
        {
            P_SEATS(local_3c)->make_basic_info((char*)&local_64, 1);
            local_70.put_short((int)P_SEATS(local_3c)->get_unique_id() & 0xffff);
            local_70.put_int(P_SEATS(local_3c)->m_network.get_inner_ip());
            local_70.put_int(P_SEATS(local_3c)->m_network.get_ip());
            local_70.put_short(P_SEATS(local_3c)->m_network.get_port() & 0xffff);
            local_70.put_int((int)P_SEATS(local_3c)->get_acc_id());
            local_70.put_byte(P_SEATS(local_3c)->m_network.get_nat_type());
            local_70.put_int(P_SEATS(local_3c)->m_network.get_mtu());
            // ORIG 0x085d709d..0x085d70c3（P48/P0 去缓存）：条件臂与 else 臂各自
            // 独立调用 getCurCharacR() 一次，不缓存到具名局部（缓存则单 call 不同形）。
            // [SweepPvPRoom-2] P59/P292 极性：ORIG 0x085d7095 `test; setne; test; je`
            // ⇒ 源形正极性 `!= 0`（旧 `== 0` 形 4.4 产 sete、且 setne 差集 +1）。
            if (P_SEATS(local_3c)->getCurCharacR() != 0)
            {
                local_70.put_byte(*(unsigned char*)((const char*)P_SEATS(local_3c)->getCurCharacR() + 0x75));
            }
            else
            {
                local_70.put_byte(0);
            }
        }
    }
    unlock();
    local_64.finalize(true);
    local_70.finalize(true);
    lock();
    int local_44 = ((IMatch*)m_matching)->CheckJoinable(this, user);
    if (local_44 != 0)
    {
        unlock();
        return local_44;
    }
    int local_40 = -1;
    for (int local_38 = 0; local_38 < 8; local_38 = local_38 + 1)
    {
        if ((P_SEATS(local_38) == 0) && (P_TEAMS(local_38) == 0xff))
        {
            local_40 = local_38;
            P_SEATS(local_38) = user;
            user->SetPvpIndex((short)m_index);
            seat = local_38;
            break;
        }
    }
    if (local_40 == -1)
    {
        unlock();
        return 4;
    }
    user->Send(local_64);
    user->Send(local_70);
    if (m_pvpBattleMode == 1)
    {
        P_TEAMS(local_40) = 0;
        m_relayMgr.OnChangeTeam(local_40, 0);
    }
    else if (m_pvpBattleMode == 4)
    {
        P_TEAMS(local_40) = 0;
    }
    else if (flag)
    {
        int local_34 = 1;
        P_TEAMS(local_40) = 1;
        m_relayMgr.OnChangeTeam(local_40, 1);
    }
    else
    {
        local_7c[0] = 0;
        local_7c[1] = 0;
        local_7c[2] = 0;
        for (int local_28 = 0; local_28 < 8; local_28 = local_28 + 1)
        {
            if ((0 < P_TEAMS(local_28)) && (P_TEAMS(local_28) < 3))
            {
                local_7c[P_TEAMS(local_28)] = local_7c[P_TEAMS(local_28)] + 1;
            }
        }
        int local_30 = 8;
        int local_2c = 1;
        for (int local_24 = 1; local_24 < 3; local_24 = local_24 + 1)
        {
            if (local_7c[local_24] <= local_30)
            {
                local_2c = local_24;
                local_30 = local_7c[local_24];
            }
        }
        P_TEAMS(local_40) = local_2c;
        m_relayMgr.OnChangeTeam(local_40, local_2c);
    }
    unlock();
    local_64.clear();
    local_64.put_header(0, 2);
    local_64.put_byte(1);
    local_64.put_short(1);
    user->make_basic_info((char*)&local_64, 1);
    local_64.finalize(true);
    send_to_pvp(local_64);
    local_70.clear();
    local_70.put_header(0, 0xb);
    local_70.put_byte(1);
    local_70.put_short((int)user->get_unique_id() & 0xffff);
    local_70.put_int(user->m_network.get_inner_ip());
    local_70.put_int(user->m_network.get_ip());
    local_70.put_short(user->m_network.get_port() & 0xffff);
    local_70.put_int((int)user->get_acc_id());
    local_70.put_byte(user->m_network.get_nat_type());
    local_70.put_int(user->m_network.get_mtu());
    // ORIG 0x085d..（P48/P0 去缓存）：条件臂与 else 臂各自独立调用
    // getCurCharacR() 一次（Ghidra C 双臂 getter），不缓存到具名局部。
    // [SweepPvPRoom-2] P59/P292 极性：ORIG 0x085d755c `test; setne; test; je`
    // ⇒ 源形正极性 `!= 0`（旧 `== 0` 形 4.4 产 sete）。
    if (user->getCurCharacR() != 0)
    {
        local_70.put_byte(*(unsigned char*)((const char*)user->getCurCharacR() + 0x75));
    }
    else
    {
        local_70.put_byte(0);
    }
    local_70.finalize(true);
    send_to_pvp(local_70);
    local_64.clear();
    local_64.put_header(1, 0x36);
    local_64.put_byte(1);
    lock();
    for (int local_20 = 0; local_20 < 8; local_20 = local_20 + 1)
    {
        local_64.put_byte(P_SEATSTATE(local_20));
    }
    unlock();
    local_64.finalize(true);
    user->Send(local_64);
    user->m_winPointBuffer = 0;   // ORIG CUser.h:2282 具名字段
    m_joinCount = m_joinCount + 1;
    m_relayMgr.OnJoinRoom(seat);
    ((IMatch*)m_matching)->OnJoinRoom(user);
    return 0;
}

// [b28d-7 N82附] ORIG 0x085d7f6c：具名局部落 -0x44/-0x40/-0x3c，4.4 同集合落 -0x2c/-0x28/-0x24（整体浅移 0x18，帧同 171）；同 TU callee 定义位置已核（P210 不适用）⇒ 槽分配策略差封顶，P167 CLEAR。
// [b28d-solo-PvP_Room] N82附: ORIG 0x085d7f6c 具名局部槽整体浅移 0x18（帧同 171）
int PvP_Room::leave_room(CUser* user, bool& flag)
{
    // [b23S3C] P37/P52 日志形：ORIG ctor 前 6 值分两路物化 —— waiter/flag/mode
    // 落槽 -0x44/-0x40/-0x3c（具名局部），index/name/acc 经 callee-saved
    // edi/esi/ebx 跨 ctor 调用缓存（register 形）；varargs 装载序
    // ebx=acc,esi=name,edi=index,槽 mode/flag/waiter。
    // P16/P56：fail 分支直接 `return -1`（ORIG mov$-1;jmp epilogue，
    // 非槽写-1再共享尾读 —— 旧形多 1 条即 sheet gap=-1 来源）。
    int local_44 = get_waiter_count();
    int local_40 = flag;
    int local_3c = get_pvp_battle_mode();
    register int roomIndex = get_index();
    register const char* characName = user->getCurCharacName();
    register const char* accName = user->get_acc_name();
    CSwitchLog(__PRETTY_FUNCTION__, 0x6c2, 0, 0)("pvp@log %s,%s,room(%d),%d,%d,%d",
             accName, characName, roomIndex, local_3c, local_40, local_44);
    lock();
    if (m_seats[7] == user)
    {
        select_new_manager();
        flag = true;
    }
    int local_24 = get_user_seat(user);
    if (local_24 < 0)
    {
        return -1;
    }
    {
        if (get_state() == 2)
        {
            GiveOuterPenalty(user);
        }
        int local_20 = get_team(local_24);
        user->SetPvpIndex(-1);
        P_SEATSTATE(local_24) = 0;
        P_TEAMS(local_24) = 0xff;
        P_SEATS(local_24) = 0;
        m_joinCount = m_joinCount - 1;
        P_WIN(local_24) = 0;
        P_TEAMWIN(local_24) = 0;
        unlock();
        user->getHackAnalyzer()->reportHackInfo();
        user->getHackAnalyzer()->resetHackInfo();
        if (m_state == 2)
        {
            // [b23S3C] P9/P159：check_winner 头改 bool（ORIG 体尾 movw $0,%ax），
            // 调用点裸形即复现 ORIG test %al 直判。
            if (check_winner())
            {
                pvp_request_pvp_rank();
            }
        }
        m_relayMgr.OnLeaveRoom(local_24);
        ((IMatch*)m_matching)->OnLeaveRoom(user, this, local_20, local_24);
    }
    return local_24;
}

CUser* PvP_Room::destroy_room(int idx)
{
    // ORIG 0x085d81d0：get_index() 在 CSwitchLog 构造前求值并以 callee-saved
    // 跨构造调用缓存（ORIG +0x07..+0x12），日志首参为 __PRETTY_FUNCTION__
    // 局部静态符号（_ZZ…E19__PRETTY_FUNCTION__，P118/P139）。
    register int roomIdx = get_index();
    CUser* local_14;
    {
        CSwitchLog local_24(__PRETTY_FUNCTION__, 0x721, 0, 0);
        local_24("pvp@log room(%d), %d", roomIdx, idx);
    }
    local_14 = P_SEATS(idx);
    if (local_14 == 0)
    {
        for (int local_10 = 0; local_10 <= 7; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) != 0)
            {
                local_14 = P_SEATS(local_10);
                break;
            }
        }
    }
    reset();
    return local_14;
}

// [b28d-solo-PvP_Room] N-P223: ORIG 0x085dd754 je 落 0x085dd809 nop 岛、jne 落 addl；goto/continue 四形实测 +2
int PvP_Room::Disconnect(CUser* user)
{
    // [b28d-7 AE] ORIG 0x085dd774 je 落 nop 岛（0x085dd809）、0x085dd789 jne
    // 落循环尾 addl（0x085dd80a）；双 continue + && 短路形经 446h46 路由即逐条
    // 全等（goto 标签串实测反劣 +1 jmp，已回退）。命中座后无条件 return 1。
    // [b28d-solo-PvP_Room] 复验：ORIG 0x085dd809 nop 占位岛（je 目标）与
    // 0x085dd80a addl（jne 目标）为 4.1 短路 && 的双落点；P223 否定 goto 尾标签
    // 形实测 +2（nop;jmp;nop 中继），共享标签/双 continue/while+goto 四形均 +2
    // ⇒ 单源岛合并负例封顶；if 包裹形与 ORIG 仅差该 nop（60 vs 61）。
    for (int i = 0; i < 8; i = i + 1)
    {
        if ((P_SEATS(i) != 0) && (P_SEATS(i) == user))
        {
            ((IMatch*)m_matching)->OnWalkOut(this, i);
            if (!WalkOut(P_SEATS(i), i, ENUM_SEAT_STATE_0xfe,
                         (ENUM_WALKOUT_TYPE)3))
            {
                ClearPvPRoom(user);
            }
            return 1;
        }
    }
    return 0;
}

int PvP_Room::SendWalkOut(int seat, ENUM_WALKOUT_TYPE walkoutType)
{
    PacketGuard local_18;
    local_18.put_header(0, 10);
    local_18.put_byte(seat);
    local_18.put_byte((int)walkoutType);
    local_18.finalize(true);
    send_to_pvp(local_18);
    return 1;
}

bool PvP_Room::WalkOut(CUser* user, int seat, ENUM_SEAT_STATE state,
                      ENUM_WALKOUT_TYPE walkoutType)
{
    // 残余为 N7 互换（虚调用 callee 寄存器跨版本分配，见 get_user_list 注释）。
    PacketGuard local_20;
    CUser* local_24 = 0;
    bool local_25 = false;
    bool local_26 = false;
    bool local_27 = false;
    int local_14 = get_team(seat);
    SendWalkOut(seat, walkoutType);
    int local_10 = set_seat_state(user, seat, state, &local_24, local_25, local_26, local_27);
    if ((get_waiter_count() == 0) && (local_27 == false))
    {
        LogManager::logFormat(1, "pvp.cpp",
                              __PRETTY_FUNCTION__,
                              0x1210, "PvP Room(%d) is empty!!!", get_index());
    }
    if (local_10 == 0)
    {
        if (local_24 != 0)
        {
            ((IMatch*)m_matching)->OnLeaveRoom(local_24, this, local_14, seat);
            G_GameWorld()->out_from_pvp(local_24);
        }
        if (local_25 != 0)
        {
            local_20.clear();
            make_seat_info((char*)&local_20, seat);
            G_GameWorld()->send_all(local_20);
        }
        if (local_26 != 0)
        {
            local_20.clear();
            make_state_info((char*)&local_20);
            G_GameWorld()->send_all(local_20);
        }
        if (local_27 != 0)
        {
            return 0;
        }
    }
    return 1;
}

// [b28d-7 N95] ORIG 0x085dd070：局部槽 -0x10/-0x1c，4.4 -0xc/-0x24（槽基差 8）；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] 本轮：P292/P248 档① check_authority int→bool（ORIG 调用点
// `xor $0x1; test %al,%al; je`）⇒ 254→253；残余 CheckSeat 需跨泳道 bool 化（已上报 Main）
// + N95 槽基差 8。
bool PvP_Room::OnSetPVPSeatState(CUser* user, int seat, ENUM_SEAT_STATE state,
                                ENUM_WALKOUT_TYPE walkoutType)
{
    CUser* local_24;
    bool local_25;
    bool local_26;
    bool local_27;
    int local_10;
    if (user == 0)
    {
        return 1;
    }
    PacketGuard local_20;
    int local_14 = 0;
    if (check_authority(user, seat) == 0)
    {
        local_14 = 8;
        goto LAB_085dd357;
    }
    if (((IMatch*)m_matching)->CheckSeat((int)state) == 0)
    {
        local_14 = 8;
        goto LAB_085dd357;
    }
    if (m_state == 2)
    {
        local_14 = 0x13;
        goto LAB_085dd357;
    }
    local_24 = 0;
    local_25 = false;
    local_26 = false;
    local_27 = false;
    local_10 = get_team(seat);
    if (state == ENUM_SEAT_STATE_0xfe)
    {
        if (walkoutType == (ENUM_WALKOUT_TYPE)3)
        {
            SendWalkOut(seat, walkoutType);
        }
        else if ((m_seats[7] == user) && (P_SEATS(seat) != 0) &&
                 (P_SEATS(seat) != user))
        {
            SendWalkOut(seat, walkoutType);
        }
    }
    local_14 = set_seat_state(user, seat, state, &local_24, local_25, local_26, local_27);
    if ((get_waiter_count() == 0) && (local_27 == false))
    {
        LogManager::logFormat(1, "pvp.cpp",
                              __PRETTY_FUNCTION__,
                              0x11be, "PvP Room(%d) is empty!!!", get_index());
    }
    if (local_14 == 0)
    {
        if (local_24 != 0)
        {
            ((IMatch*)m_matching)->OnLeaveRoom(local_24, this, local_10, seat);
            G_GameWorld()->out_from_pvp(local_24);
            local_24->insert_pvp_masterid_walkingout_me((unsigned)user->getCurCharacNo());
        }
        if (local_25 != 0)
        {
            local_20.clear();
            make_seat_info((char*)&local_20, seat);
            G_GameWorld()->send_all(local_20);
        }
        if (local_26 != 0)
        {
            local_20.clear();
            make_state_info((char*)&local_20);
            G_GameWorld()->send_all(local_20);
        }
        if (local_27 != 0)
        {
            return 0;
        }
    }
LAB_085dd357:
    if (local_14 > 0)
    {
        local_20.put_header(1, 0x37);
        local_20.put_byte(0);
        local_20.put_byte(local_14);
        local_20.finalize(true);
        user->Send(local_20);
    }
    return 1;
}

// [b28d-7 N82] ORIG 0x085d77c0：push %edi/%esi + 槽 -0x3c/-0x40/-0x44，4.4 无 push 且槽 -0x1c（帧 0x6c vs 0x64）；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] 本轮（403→406）：P227 CSwitchLog 全表达式临时链 ⇒ get_acc_name→%esi/
// getCurCharacName→%ebx 提前到 ctor 前（与 ORIG 同向）；残余 N82 槽号（-0x3c..-0x44 vs
// -0x14..-0x1c）+ 块序重排。
// [SweepPvPRoom] 406→409（ORIG 409，gap 归零）：P231 裸 register 使 idx 入 %edi 跨
// CSwitchLog ctor 保活（补回 push/pop %edi）+ P97 两处 change_team 第二实参改形参
// state（ORIG 0x085d78b2 `mov 0x14(%ebp),%eax; mov %eax,0x8(%esp)`，旧形硬编码
// 0xfe/0xff 立即数）。残余 = N82 槽号整体偏移 + P251/P59（if 链 vs switch 寄存器比较
// `mov 0x14(%ebp),%eax; cmp $0xfe,%eax`）+ 1 处 test/sete 物化。
int PvP_Room::set_seat_state(CUser* user, int seat, ENUM_SEAT_STATE state,
                             CUser** out, bool& a, bool& b, bool& c)
{
    if (user == 0)
    {
        return 0;
    }
    int local_24;
    {
        // ORIG(4.1) 实参先求值（后四入槽/前三入寄存器）→ ctor 后推参，保形直写
        int wc = get_waiter_count();                    // -0x44
        int st = (int)state;                            // -0x40
        int mode = get_pvp_battle_mode();               // -0x3c
        // [SweepPvPRoom] P231 裸 register（非 asm 绑定，P0 合规）：ORIG 0x085d7806
        // `call get_index; mov %eax,%edi` + 尾 `mov %edi,0x10(%esp)`，值跨 CSwitchLog
        // ctor 保活于 callee-saved %edi（配套 push/pop %edi）；具名 int 形落 -0x10 槽
        // 多一条 store+reload。
        register int idx = get_index();                 // ORIG %edi
        // [b28d-solo-PvP_Room] P227：ORIG 0x085d77c0 在 ctor 前求 get_acc_name→%esi、
        // getCurCharacName→%ebx 并跨 ctor 保活（具名对象形 4.4 恒 ctor 后求值）⇒
        // 全表达式临时链。
        CSwitchLog(__PRETTY_FUNCTION__, 0x594, 0, 0)(
            "pvp@log %s,%s,room(%d),%d,%d,%d,%d", user->get_acc_name(),
            user->getCurCharacName(), idx, mode, seat, st, wc);
    }
    // [SweepPvPRoom-2] P272/P59 块序交换：ORIG 0x085d7888 `mov 0x14(%ebp),%eax;
    // cmp $0xfe,%eax; je <A 体>` + `cmp $0xff,%eax; jne <C 体>`（B 体直落）；
    // 反写 `if (state != 0xfe) { if (state == 0xff) B else C } else { A }` 复现该
    // 物理序（旧 `if (state == 0xfe) A else if …` 形 4.4 把 A 体直落、B/C 后置）。
    // [SweepPvPRoom-2] P286/P324：ORIG 0x085d7888 的「形参载入 + 寄存器比较」
    // 链（`mov 0x14(%ebp),%eax; cmp $0xfe,%eax; je A; cmp $0xff,%eax; jne C`）
    // 由 switch 复现（探针 probe_enum3 逐条同形）；叶块物理序 = 书写序
    // ⇒ case 顺序 0xff / 0xfe / default（旧 if 反写形 4.4 恒 cmpl 内存直比）。
    switch (state)
    {
    case (ENUM_SEAT_STATE)0xff:
        {
            if (m_seats[7] != user)
            {
                return 8;
            }
            // [SweepPvPRoom] P97：ORIG 同形（第二实参取形参 state，非立即数 0xff）。
            change_team(seat, (int)state);
            a = true;
        }
        break;
    case ENUM_SEAT_STATE_0xfe:
        {
        if (m_state == 2)
        {
            if (P_SEATS(seat) == 0)
            {
                return 4;
            }
            if (m_seats[7] == P_SEATS(seat))
            {
                select_new_manager();
                a = true;
                b = true;
            }
            *out = P_SEATS(seat);
            GiveOuterPenalty(*out);
            P_SEATS(seat)->SetPvpIndex(-1);
            local_24 = get_team(seat);
            lock();
            P_SEATSTATE(seat) = 0;
            P_TEAMS(seat) = 0xff;
            P_SEATS(seat) = 0;
            m_joinCount = m_joinCount - 1;
            P_WIN(seat) = 0;
            unlock();
            m_relayMgr.OnLeaveRoom(seat);
            ((IMatch*)m_matching)->OnLeaveRoom(*out, this, local_24, seat);
            if (get_waiter_count() == 0)
            {
                reset();
                a = true;
                b = true;
                c = true;
            }
            else
            {
                if (check_winner() != 0)
                {
                    b = true;
                    pvp_request_pvp_rank();
                }
                a = true;
            }
        }
        else if (m_seats[7] == user)
        {
            if (P_SEATS(seat) == 0)
            {
                if (is_closeable_seat_state() == 0)
                {
                    return 0x13;
                }
                // [SweepPvPRoom] P97：ORIG 0x085d78b2 `mov 0x14(%ebp),%eax;
                // mov %eax,0x8(%esp)` ⇒ 第二实参是形参 state，非硬编码 0xfe。
                change_team(seat, (int)state);
                a = true;
            }
            else
            {
                P_SEATS(seat)->SetPvpIndex(-1);
                if (P_SEATS(seat) == m_seats[7])
                {
                    select_new_manager();
                    b = true;
                }
                *out = P_SEATS(seat);
                P_SEATSTATE(seat) = 0;
                P_TEAMS(seat) = 0xff;
                P_SEATS(seat) = 0;
                m_relayMgr.OnChangeTeam(seat, 0xff);
                if (get_waiter_count() == 0)
                {
                    reset();
                    a = true;
                    b = true;
                    c = true;
                }
                else
                {
                    a = true;
                    b = true;
                }
            }
        }
        else if (P_SEATS(seat) != 0)
        {
            P_SEATS(seat)->SetPvpIndex(-1);
            *out = P_SEATS(seat);
            int local_20 = get_team(seat);
            P_SEATSTATE(seat) = 0;
            P_TEAMS(seat) = 0xff;
            P_SEATS(seat) = 0;
            m_joinCount = m_joinCount - 1;
            m_relayMgr.OnLeaveRoom(seat);
            ((IMatch*)m_matching)->OnLeaveRoom(*out, this, local_20, seat);
            if (get_waiter_count() == 0)
            {
                reset();
                a = true;
                b = true;
                c = true;
            }
            else
            {
                a = true;
            }
        }
        }
        break;
    default:
        {
            if (P_SEATS(seat) != user)
            {
                return 8;
            }
            change_team(seat, (int)state);
            a = true;
        }
        break;
    }
    return 0;
}

// [b28d-solo-PvP_Room] 已归零(AE): P248 档① check_team_pvp int→bool（调用点 xor $1 取反形）+ 正极性 if(check_start_state()) + ready 形参直用（P0 槽回读 -0x1c）
int PvP_Room::set_ready_state(CUser* user, bool ready)
{
    if (((IMatch*)m_matching)->OnReady(user, ready) != 1)
    {
        return 0;
    }
    PacketGuard local_1c;
    if (m_seats[7] == user)
    {
        if (ready)
        {
            lock();
            P_SEATSTATE(m_managerSeat) = (ENUM_SEAT_STATE)ready;
            m_relayMgr.OnReady(m_managerSeat, ready);
            unlock();
            if (check_start_state())
            {
                if (!check_team_pvp())
                {
                    return 0xe;
                }
                make_ready_info((char*)&local_1c, m_managerSeat);
                send_to_pvp(local_1c);
                return 0xffffffff;
            }
            P_SEATSTATE(m_managerSeat) = 0;
            m_relayMgr.OnReady(m_managerSeat, false);
            return 0x16;
        }
    }
    else
    {
        int local_10 = 0;
        lock();
        for (local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) == user)
            {
                P_SEATSTATE(local_10) = ready ? 1 : 0;
                m_relayMgr.OnReady(local_10, ready);
                break;
            }
        }
        unlock();
        if (local_10 < 8)
        {
            make_ready_info((char*)&local_1c, local_10);
            send_to_pvp(local_1c);
        }
    }
    return 0;
}

// [b28d-7 P75] ORIG 0x085dbcce：常量/形参实参点先载 receiver(-0xc→%eax) 再 store 常量槽，4.4 恒反序；指令集合/语义全等（P75 跨版本调度），P167 CLEAR。
// [b28d-solo-PvP_Room] P75: ORIG 0x085dbcce 实参装载时刻 %edx vs %eax 跨版本调度
int PvP_Room::make_ready_info(char* buf, int seat)
{
    // P75/N7 封顶（ORIG 0x085dbcd4）：ORIG(4.1) 对常量/形参实参点先载 receiver
    // (-0xc→%eax) 再 store 常量槽，计算值实参点先物化值(%edx) 后载 receiver；
    // 4.4 恒先 store 实参后载 receiver 且 scratch 用 %eax 顶替 %edx。指令集合/
    // 语义全等，装载时刻与寄存器角色为跨版本调度固有差（P75 判例），勿再耗轮次。
    InterfacePacketBuf* local_f4 = (InterfacePacketBuf*)buf;
    local_f4->put_header(0, 0x2c);
    local_f4->put_byte(seat);
    local_f4->put_byte(P_SEATSTATE(seat));
    local_f4->finalize(true);
}

// [b28d-7 P75] ORIG 0x085db7f6：成员值先物化 %edx 再压参（`mov (%eax),%edx; mov %edx,0x4(%esp)`），4.4 折成 eax 直压（P75 装载时刻/寄存器角色）⇒ 封顶，P167 CLEAR。
// [b28d-solo-PvP_Room] 本轮：P59/P272 —— 循环内 `P_SEATS(i)==0` 改正极性 `!=0`（ORIG
// `test; je <T>` 非零分支直落）⇒ 分支布局归位；残余仅 P75 装载序。
// [SweepPvPRoom] 残余 1 条 = N4/P30：ORIG 0x085db860 `lea 0x9(%eax),%ecx`（put_str 实参
// 单条取址）vs 4.4 `mov this,%edx; add $0x9,%edx`；`(char*)&m_roomName`/`&m_roomName[0]`/
// `m_roomName + 0` 三形实测均仍出 add（139 不变）⇒ add-vs-lea 伪影封顶。
int PvP_Room::make_room_info(char* buf)
{
    InterfacePacketBuf* local_ec = (InterfacePacketBuf*)buf;
    local_ec->put_short(m_index);
    local_ec->put_byte((int)m_mode8);
    if (m_mode8 == 0)
    {
        size_t len = strlen(m_roomName);
        local_ec->put_int((int)len);
        // [SweepPvPRoom] N4/P30 封顶：ORIG 0x085db860 `lea 0x9(%eax),%ecx`（单条取址）
        // vs 4.4 `mov this,%edx; add $0x9,%edx`；`(char*)&m_roomName`/`&m_roomName[0]`/
        // `m_roomName + 0` 三形实测均仍出 add（139 不变）⇒ add-vs-lea 伪影。
        local_ec->put_str(m_roomName, (int)len);
    }
    local_ec->put_byte(m_state);
    local_ec->put_byte(m_managerSeat);
    // [SweepPvPRoom] P200：去掉冗余 (int) cast ⇒ short 实参直传复现 ORIG
    // `movswl %ax,%edx`（cast 形恒退化成 `cwtl` 单条，少 1 条）。
    local_ec->put_short(m_requestedMapNo);
    local_ec->put_byte(m_pvpBattleMode);
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        local_ec->put_byte(P_TEAMS(local_10));
        // [b28d-solo-PvP_Room] P59/P272：ORIG 0x085db7f6 为 `test; je <T>`，
        // 非零分支直落、零分支落 <T> ⇒ 源形须正极性 `!= 0`（旧 `== 0` 反产 jne 布局）。
        if (P_SEATS(local_10) != 0)
        {
            local_ec->put_short((int)P_SEATS(local_10)->get_unique_id() & 0xffff);
            local_ec->put_byte((int)P_SEATS(local_10)->isAllowLocationGeoState());
        }
        else
        {
            local_ec->put_short(0xffff);
            local_ec->put_byte(0);
        }
    }
    local_ec->put_byte(m_isExistPassword);
    local_ec->put_int(((IMatch*)m_matching)->GetIndentification());
}

// [b28d-7 N82附] ORIG 0x085db9e2：vptr/名称结果以 %edi/%esi/%ebx callee-saved 跨 3 调用缓存（push 对 +2），4.4 无 push 且改槽中转（199/193）；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] N82附: ORIG 0x085db9e2 vptr/名称结果 callee-saved 跨 3 调用缓存，4.4 改槽中转
// [SweepPvPRoom] 193→198（ORIG 199）：P227 全表达式临时链复现 cMyTrace ctor 前求
// get_unique_id→%esi / team→%ebx 并跨 ctor 保活（补回 push/pop %esi/%ebx）；旧
// 「N82附 callee-saved 缓存封顶」判词作废。残余 1 条 = ORIG 0x085dba25 `mov %eax,%edx`
// （成员实参中转，P75/N7 装载序：多重集其余差异全为 %edx↔%eax 寄存器名互换）。
void PvP_Room::make_seat_info(char* buf, int seat)   // [b28c-16 SIGFIX] ORIG 串 void
{
    // ORIG 0x085db9e2（199；本侧 193，-6 全为 N 类形态差，multiset 核对无语义
    // 缺口）：ORIG 将 vptr 读值/getCurCharacName/get_acc_name 结果以 %edi/%esi/
    // %ebx callee-saved 跨 3 次调用缓存（push 对 +2、`mov (%eax),%edx` 直落
    // outgoing 槽 vs 4.4 `mov…,%eax; mov %eax,…` 两段，N7/N18a），错误码/日志
    // 串/PRETTY 符号逐条一致。
    // [b28d-t3-PoolBigThree] 121→**106**：else 段 `P_SEATS(seat)` 检查改正极性
    // `!=0`（ORIG 0x085dbc6x `je 85dbc83` 零分支落尾岛、uid/geo fall-through，
    // 旧 `==0` 形两分支体与 ORIG 错位）。残余 = N7/N18a callee-saved 缓存（上）。
    InterfacePacketBuf* local_18 = (InterfacePacketBuf*)buf;
    local_18->put_header(0, 0x2b);
    local_18->put_short(m_index);
    local_18->put_byte(m_pvpBattleMode);
    if (seat == 0xffff)
    {
        int local_14 = get_waiter_count();
        local_18->put_byte(local_14);
        if (local_14 == 0)
        {
            LogManager::logFormat(1, "pvp.cpp", __PRETTY_FUNCTION__,
                                  0xe66, "Send Member 0 PvpRoom %d State(%d)", m_index, m_state);
        }
        lock();
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) != 0)
            {
                local_18->put_byte(local_10);
                local_18->put_byte(P_TEAMS(local_10));
                local_18->put_short((int)P_SEATS(local_10)->get_unique_id() & 0xffff);
                local_18->put_byte((int)P_SEATS(local_10)->isAllowLocationGeoState());
                // [SweepPvPRoom] P227 全表达式临时链（旧 N82附 判词作废）：ORIG
                // 0x085dbb5e `call get_unique_id; movzwl %ax,%esi` 与 0x085dbb6f
                // `mov (%eax,%edx,4),%ebx`（team）都在 ctor（0x085dbb90）之前求值并
                // 以 callee-saved 跨 ctor 保活；两语句形把 ctor 提到求值之前 ⇒ 缺
                // push/pop %esi/%ebx 与两条 callee-saved 装载（gap 6 的主体）。
                cMyTrace(__PRETTY_FUNCTION__, 0xe74, 0)(
                    "RELAY_BATTLE SeatInfo player(%d), team(%d), uid(%d)\n",
                    local_10, P_TEAMS(local_10),
                    (int)P_SEATS(local_10)->get_unique_id() & 0xffff);
            }
        }
        unlock();
    }
    else
    {
        local_18->put_byte(1);
        lock();
        local_18->put_byte(seat);
        local_18->put_byte(P_TEAMS(seat));
        // [b28d-t3-PoolBigThree] P59 块序：ORIG 0x085dbc6x `je 85dbc83`（零分支
        // 落尾岛）非零分支 fall-through 直落 uid/geo ⇒ 正极性 `!= 0`。
        if (P_SEATS(seat) != 0)
        {
            local_18->put_short((int)P_SEATS(seat)->get_unique_id() & 0xffff);
            local_18->put_byte((int)P_SEATS(seat)->isAllowLocationGeoState());
        }
        else
        {
            local_18->put_short(0xffff);
            local_18->put_byte(0);
        }
        unlock();
    }
    local_18->finalize(true);
}

// [b28d-7 P75] ORIG 0x085dbd3a：receiver 先载（`mov -0xc(%ebp),%eax` 于常量槽 store 之前），4.4 恒反序（P75 跨版本装载时刻），P167 CLEAR。
// [b28d-solo-PvP_Room] P75: ORIG 0x085dbd3a receiver 先载/常量槽 store 序跨版本调度
// [SweepPvPRoom] 残余 1 条 = ORIG 0x085dbdc4 `mov %eax,%edx`（m_pvpBattleMode 实参
// 中转；同段其余成员走 `mov disp(%eax),%edx` 单段）⇒ P75/N18a 装载序封顶；short 实参
// `(int)` cast 已删（P200，ORIG `movswl %ax,%edx` 复现）。
int PvP_Room::make_state_info(char* buf)
{
    InterfacePacketBuf* local_f4 = (InterfacePacketBuf*)buf;
    local_f4->put_header(0, 0x2a);
    local_f4->put_short(m_index);
    local_f4->put_byte(m_state);
    local_f4->put_byte(m_managerSeat);
    // [SweepPvPRoom] P200：ORIG 0x085dbd6x `movswl %ax,%edx`（short 直传）；
    // 冗余 (int) cast 退化成 `cwtl`（gap 1）。
    local_f4->put_short(m_requestedMapNo);
    local_f4->put_byte(m_pvpBattleMode);
    local_f4->put_int(((IMatch*)m_matching)->GetIndentification());
    local_f4->finalize(true);
}

void PvP_Room::get_room_info(PVP_ROOM_INFO* out)
{
    out->title = m_roomName;
    out->byte4 = m_mode8;
    out->short6 = m_requestedMapNo;
    out->byte8 = m_isExistPassword;
    out->passwd = m_password;
}

void PvP_Room::get_user_list(void* out)
{
    // N7 封顶：ORIG 虚表槽函数指针落 %ecx、receiver 链 %eax；4.4 恒 eax/edx 形，
    // 临时/引用/限定名均无效（负例表 N7）。
    ((IMatch*)m_matching)->GetUserList(out);
}

void PvP_Room::send_to_pvp(PacketGuard& guard)
{
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) != 0)
        {
            P_SEATS(local_10)->Send(guard);
        }
    }
}

// [b28d-solo-PvP_Room] 两循环各用「P226 语句表达式包前两条件 + 嵌套 if 包第三条件」
// 复现 ORIG 0x085dbeb9/0x085dbf36 的「先物化 bool 岛、再比较第三条件」布局。
// [SweepPvPRoom] P223 变体（b28c C28L19 裸早返产尾岛）：ORIG 0x085dbe73
// `jle 085dbf7c`（else 岛 = nop + jmp 085dbf80）与 0x085dbf7a `jmp 085dbf80`
// （第二循环出口）、0x085dbf7f nop（local_18==0 岛）共 2 条 jmp；旧 if 包裹形被
// 4.4 epilogue-threading 合并（旧判 N121 的 2 条缺口）。改裸早返 `if (mode<=1) return;`
// 复现 else 岛 + body 尾 jmp。
void PvP_Room::send_to_pvp_team(PacketGuard& guard, CUser* user)
{
    // [b28d-solo-PvP_Room] ORIG 0x085dbe64 用 jle（有符号）比较；m_pvpBattleMode
    // 字段为 unsigned（set_pvp_mode store 序需要，P211），此处 (int) 收窄比较域。
    if ((int)m_pvpBattleMode <= 1)
        return;
    {
        int local_18 = 0;
        for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
        {
            // [b28d-solo-PvP_Room] ORIG 0x085dbeb9：前两条件先物化 bool 岛
            // （mov$1/jmp/mov$0/test/je），随后才是 `P_SEATS==user` 比较 ⇒
            // 语句表达式包前两条件 + 嵌套 if 包第三条件（直接 && 链恒把物化推迟
            // 到最后，物化位置错）。
            if (({ (P_SEATS(local_14) != 0) &&
                   ((char)IsPvpObserver(local_14) != 1); }))
            {
                if (P_SEATS(local_14) == user)
                {
                    local_18 = P_TEAMS(local_14);
                    break;
                }
            }
        }
        if (local_18 == 0)
            return;
        // [SweepPvPRoom-2] P223 变体（裸早返产尾岛）：`if (local_18 == 0) return;`
        // 复现 ORIG 0x085dbf7f nop 岛（je 落点）+ 0x085dbf7a jmp（第二循环出口）
        // 双岛形（94=94 AE）；旧 `if (local_18 != 0) { … }` 包裹形被 4.4 合并成一个
        // 尾岛（92 条）。
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            // 同第一循环：ORIG 0x085dbf36 段亦为「前两条件物化岛 + 第三条件
            // 独立比较」⇒ 语句表达式 + 嵌套 if。
            if (({ (P_SEATS(local_10) != 0) &&
                   ((char)IsPvpObserver(local_10) != 1); }))
            {
                if (P_TEAMS(local_10) == local_18)
                {
                    P_SEATS(local_10)->Send(guard);
                }
            }
        }
    }
}

int PvP_Room::ClearPvPRoom(CUser* user)
{
    destroy_room(0);
    G_CGameManager()->PutPvp(this);
    PacketGuard local_18;
    local_18.clear();
    make_state_info((char*)&local_18);
    G_GameWorld()->send_all(local_18);
}

int PvP_Room::change_team(int seat, int team)
{
    lock();
    P_TEAMS(seat) = team;
    m_relayMgr.OnChangeTeam(seat, team);
    unlock();
}

int PvP_Room::select_new_manager()
{
    for (int local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) != 0)
        {
            if (P_SEATS(local_8) != m_seats[7])
            {
                m_seats[7] = P_SEATS(local_8);
                m_managerSeat = local_8;
                P_SEATSTATE(local_8) = 0;
                return 1;
            }
        }
    }
    return 0;
}

// [b28d-7 N-P223] ORIG 0x085d7df0：双守卫 je/jne 分落 +0x15c/+0x15d 两独立 landing，4.4 合并共享出口 +1 jmp；continue 形与 goto 标签串形实测均 +1（P223 单源岛合并负例）⇒ 封顶。
// [b28d-solo-PvP_Room] N-P223: ORIG 0x085d7d54 双守卫分落 +0x15c/+0x15d 两独立 landing，4.4 合并共享出口
int PvP_Room::change_manager_to_winner()
{
    // ORIG 0x085d7d54（81；本侧 83，+2=N 落点：ORIG seat==0 skip 的 je 与
    // is_last_win!=true skip 的 jne 分落 +0x15c/+0x15d 两独立 landing（其间
    // 1 条 nop/重叠指令布局产物），4.4 合并共享出口+中继 jmp。bVar 常量岛/
    // !=1→xor 物化/嵌套实参消槽三形已应用，141→83。）
    // [batch-19 N 封顶 ndiff=1] 再实测三形均不优于双 continue：
    // ① 合并 `if (seat!=0 && is_last_win(i)==true)` → 84 insn（丢 `xor$1`
    //   物化且中继仍在）；② 纯嵌套 `if(seat){if(==true){body}}` → 79 insn
    //   （极性翻 test/je，丢 ORIG `xor$1;jne`）；③ `!is_last_win(i)` 替
    //   `!= true` → 83 insn 逐条同（xor$1 保留），无增益。
    if (m_seats[7] == 0)
    {
        return 0;
    }
    if (G_GameWorld()->GetChannelType() == 0xf)
    {
        return 1;
    }
    if (G_GameWorld()->GetChannelType() == 7 ||
        G_GameWorld()->IsSchoolPvPChannel() != 0)
    {
        return 1;
    }
    if (is_last_win(m_managerSeat))
    {
        return 1;
    }
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) == 0)
            goto CMW_NEXT;
        if (is_last_win(local_10) != true)
            goto CMW_NEXT;
        m_seats[7] = P_SEATS(local_10);
        m_managerSeat = local_10;
        return 1;
    CMW_NEXT:
        ;
    }
    return 0;
}

// [b28d-7 N95] ORIG 0x085dcaf2：对象槽 -0xb0 与局部 -0xc，4.4 整体深移 8（-0xb8/-0x14）；P167 CLEAR ⇒ N95 槽基封顶。
// [b28d-solo-PvP_Room] 本轮：P0 —— 去 `void* ev` 具名局部 + 裸函数指针虚调用改具型化
// `GetRepeatEvent(0x38)->IsEventing((CUser*)0)`（ORIG 值全程 %eax 无槽往返）⇒ 176→172；
// 残余 N95 槽基差 8（帧同 0xc0，P190 撑帧法不可上移）。
// [SweepPvPRoom] 172→176（ORIG 175）：P226 扩展合并短路 && —— ORIG 0x085dcba4..0x085dcbf0
// 是 `IsIntegratedPvPBaseChannel && IsEventing` 的零槽物化岛（5 条），旧形拆嵌套 if 缺该岛；
// 合并后槽基差 8→4（-0xb4 vs ORIG -0xb0）。残余 = 上述槽基差 + 1 条 `lea ...%ebx` 取址缓存
// 与 1 条 ORIG 尾 jmp（N95/P231 族）。
void PvP_Room::GiveOuterPenalty(CUser* user)   // [b28c-16 SIGFIX] ORIG 串 void
{
    PvpResultType local_b4;
    local_b4.Clear();
    user->update_pvp_rank(local_b4);
    int local_10 = GetTeamIndex(user);
    if (m_userTable.IsValidTeam(local_10) != 1)
    {
        cMyTrace local_20(__PRETTY_FUNCTION__, 0x10f8, 5);
        local_20("Invalid Team(%d)", local_10);
        goto LAB_085dcd66;
    }
    // [SweepPvPRoom] P226 扩展（旧「外层 if + 内层语句表达式」判词作废）：ORIG
    // 0x085dcba4..0x085dcbf0 是**合并短路 &&** 的零槽物化岛 —— `call
    // IsIntegratedPvPBaseChannel; test %al; je <false>; … call *%edx (IsEventing);
    // test %al; je <false>; mov $1,%eax; jmp; <false>: mov $0,%eax; test %al; je
    // <else>`；拆成嵌套 if 缺 5 条物化岛。语句表达式包住整个 &&（内不含具名变量）。
    // P0：GetRepeatEvent 返回值全程 %eax（无槽往返），虚调用是 vptr+0x34 具型化。
    if (({ G_GameWorld()->IsIntegratedPvPBaseChannel() != 0
           && GlobalData::s_event_manager->GetRepeatEvent(0x38)->IsEventing((CUser*)0) != 0; }))
    {
        if ((int)(long)user->GetServerGroup() != 0)
        {
            Packet_Server_Match_data local_2d;
            local_2d.m_serverGroup = (char)(long)user->GetServerGroup();
            local_2d.m_lose = 1;
            CStatisticServerProxy* proxy =
                GlobalData::s_statistic_proxy_mgr->GetServerProxy(
                    (ENUM_SERVER_GROUP)(long)user->GetServerGroup());
            proxy->SendPacket((char*)&local_2d, 0xd);
        }
    }
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        if ((G_GameWorld()->GetChannelType() == 4) ||
            (G_GameWorld()->GetChannelType() == 8))
        {
            user->update_pvp_point(m_userTable.GetLeavePenalty(local_10));
        }
        else if (G_GameWorld()->GetChannelType() == 6)
        {
            user->add_guild_pvp_result(-1);
            goto LAB_085dcd66;
        }
    }
    if (G_GameWorld()->IsWinPointPvPChannel() != 0)
    {
        user->add_pvp_result(false, 0);
    }
    ((IMatch*)m_matching)->OnGiveup(user);
LAB_085dcd66:
    ;
}

// [b28d-7 N7] ORIG 0x085dcf1a：key→%ebx / m_index→%esi（0x085dd01a/085dd014），4.4 反配；asm 绑定与裸 register（P231）两形实测均不翻（P248 条件②不成立）⇒ 寄存器角色互换封顶，语义全等、P167 CLEAR。
// [b28d-solo-PvP_Room] N7-N109: ORIG 0x085dcf1a key->ebx/m_index->esi，4.4 反配；(int)/unsigned 两旋钮实测不翻
bool PvP_Room::VoteLackUser(CUser* user, int vote)   // [b28c-16 SIGFIX] ORIG 串 bool
{
    if ((vote < 0) || (7 < vote))
    {
        LogManager::logFormat(1, "pvp.cpp", __PRETTY_FUNCTION__,
                              0x116d, "VoteSlotNo(%d) < 0 || MAX_PVP_PLAYER < VoteSlotNo", vote);
        return false;
    }
    CUser* u = P_SEATS(vote);
    // ORIG 085dccbe: mov 0xc(%ebp),%eax（user 先载 reg）/ cmp -0x10(%ebp),%eax
    // —— 操作数序 user==u；u==user 反产 slot 先载。
    if ((user == u) || (u == 0))
    {
        return false;
    }
    int iVar2 = get_user_seat(user);
    if ((iVar2 < 0) || (8 < iVar2))
    {
        LogManager::logFormat(1, "pvp.cpp", __PRETTY_FUNCTION__,
                              0x117b, "UserSlotNo(%d) < 0 || MAX_PVP_PLAYER < UserSlotNo", iVar2);
        return false;
    }
    m_election.Vote(iVar2, vote);
    // P24/P52：ORIG 链式内联实参（0x085dd01a），key 先物化 ebx、m_index 缓存 esi，
    // G_TimerQueue() 置后，跨调用 callee-saved 存活
    // ORIG 085dd00e/085dd014：key→%ebx、index→%esi（4.4 反产 = N7 互换，
    // asm("ebx")/裸 register 实测均不翻，P248 条件②不成立）⇒ 保持直写。
    G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1, m_index,
                                            (TIMER_MESSAGE)0x31, 0x9c4LL,
                                            (int)gen_timer_key((TIMER_MESSAGE)0x31), 0);
    return true;
}

int PvP_Room::OnConnectP2PPvPTimeout()
{
    // 残余 2 行 N7 互换（虚调用 callee 寄存器跨版本分配，见 get_user_list 注释）。
    if (!check_start_state())
    {
        ((IMatch*)m_matching)->OnConnectP2PTimeout(this);
    }
}

bool PvP_Room::OnTimerWalkOutLackUser()
{
    // ORIG 0x085dce28：Ghidra 双条件 `-1 < local_10 && -1 < local_10`——尾 cond
    // 用 not/shr$0x1f 物化（loop tail），body 头重复 `cmpl$0; js→nop 岛`
    // （P184 死块照抄）。双 guard 形（while(x>-1) + 体内 if(x<0) break）使
    // 4.4 复现全部 69 条含 not/shr 岛（合并 && 形被 bool 汇聚，P133）；
    // 残余 = OnWalkOut 虚 call callee-saved 互换 10 行（N7，同 get_user_list 判例），
    // NEAR 封顶。
    int local_10 = m_election.GetElected();
    while (local_10 > -1)
    {
        if (local_10 < 0)
            break;
        m_election.CancelCandidate(local_10);
        m_election.CancelVoter(local_10);
        ((IMatch*)m_matching)->OnWalkOut(this, local_10);
        if (!OnSetPVPSeatState(P_SEATS(local_10), local_10, ENUM_SEAT_STATE_0xfe,
                               (ENUM_WALKOUT_TYPE)3))
        {
            return 0;
        }
        local_10 = m_election.GetElected();
    }
    return 1;
}

int PvP_Room::HandleTimerUserRevive(int seat)
{
    // ORIG 085dd924/085dd927/085dd92a/085dd92d：四 skip 条件各落独立 nop 岛
    // （js/jg/je/jne→nop×4，末岛 fall merge）；P_WIN 非零跳形 = if(P_WIN) 直写。
    if (seat < 0)
        goto HTR_OUT_RANGE1;
    if (seat > 7)
        goto HTR_OUT_RANGE2;
    if (P_SEATS(seat) == 0)
        goto HTR_OUT_SEAT;
    if (P_WIN(seat))
        goto HTR_OUT_WIN;
    P_WIN(seat) = 1;
    {
        PacketGuard local_18;
        local_18.put_header(0, 0x78);
        local_18.put_byte(seat);
        local_18.put_byte(rand() % 0x100);
        local_18.finalize(true);
        send_to_pvp(local_18);
    }
HTR_OUT_WIN:
    ;
HTR_OUT_SEAT:
    ;
HTR_OUT_RANGE2:
    ;
HTR_OUT_RANGE1:
    ;
}

int PvP_Room::SkipLastRankResultTimer()
{
    // P31：ORIG 帧 0x18 vs gcc4.4 0x8，补 4 个未用局部对齐帧深（ORIG 0x085d9ba4）
    int d1;
    int d2;
    int d3;
    int d4;
    gen_timer_key((TIMER_MESSAGE)0x32);
}

int PvP_Room::CancelAllTimer()
{
    // P31：ORIG 帧 0x18 vs gcc4.4 0x8，补 4 个未用局部对齐帧深（ORIG 0x085d6c42）
    int d1;
    int d2;
    int d3;
    int d4;
    gen_timer_key((TIMER_MESSAGE)0x32);
}

int PvP_Room::end_pvp()
{
    // N7 封顶（虚调用 callee 寄存器互换，见 get_user_list 注释）。
    ((IMatch*)m_matching)->OnEndFight(this);
}

void PvP_Room::end_pvp_result()
{
    change_manager_to_winner();
    m_state = 1;
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) == 0)
        {
            continue;
        }
        P_SEATS(local_10)->set_state((ch_state)6);
    }
    SkipLastRankResultTimer();
}

int PvP_Room::send_pvp_end()
{
    m_isEndPvpBattle = 1;
    if (G_GameWorld()->GetChannelType() != 6)
    {
        update_pvp_point();
    }
    pvp_reward();
    return 1;
}

int PvP_Room::pvp_request_pvp_rank()
{
    // [b28d-7] P223：ORIG 0x085dcd92 jne 落空 then 占位岛 nop（0x085dce1f），
    // body 尾 jmp（0x085dce02）越过 PacketGuard landing pad 落 0x085dce20。
    // 复现形=否定 goto + 尾空标签（含初始化声明包入内层复合块）。
    if (m_rankRequested != 0)
        goto PVP_RANK_DONE;
    m_rankRequested = 1;
    {
        PacketGuard local_18;
        local_18.put_header(0, 0x31);
        local_18.finalize(true);
        send_to_pvp(local_18);
        m_fieldC0 = 1;
    }
PVP_RANK_DONE:
    ;
}

bool PvP_Room::request_fight(CUser* user)
{
    if ((unsigned char)m_battleEndFlag != 0)
    {
        return 1;
    }
    int iVar2 = get_user_seat(user);
    if (iVar2 < 0)
    {
        return 0;
    }
    m_relayMgr.OnRequestFight(iVar2);
    return 1;
}

void PvP_Room::revengeMission(bool flag)
{
    CMissionList_Charac* mission;
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) != 0)
        {
            mission = (CMissionList_Charac*)P_SEATS(local_10)->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
            // ORIG 085ddf7f: mov 0x8(%ebp),%eax / mov 0x4(%eax),%edx —— 成员值单步
            // 直落 %edx（技巧2 register 直落型），再统一 spill 到 0x8(%esp)。
            // 4.4 对「成员→%eax 后再挪 %edx 槽」恒物化两步（N18a 族：4.1 可单步
            // 直落任意硬寄存器，4.4 fold 仅到首寄存器），register asm 反产更差形。
            // [batch-19 复核] `static_cast`↔C 式 `(PVP_BATTLE_MODE)` 两种写法实测
            // 输出全等（45 insn），中转步与 cast 形式无关 ⇒ N18a 封顶，ndiff=3。
            mission->Update_Revenge_event(*P_SEATS(local_10),
                                          static_cast<PVP_BATTLE_MODE>(m_pvpBattleMode),
                                          flag);
        }
    }
}

// [b28d-solo-PvP_Room] AE: P248 档①/P292 已落 —— IsDeathMatchMap 返回类型 bool 化 +
// PVPMAP_DEATHMATCH_MODE 改 bool 数组 ⇒ 调用点 `call; test %al; je` 无 setne，与 ORIG
// 0x085d8b2a 全等（旧「uchar callee setne 归一封顶」判词作废）。
int PvP_Room::ChangePvPMode(int mapNo)
{
    bool local_d;
    local_d = false;
    // [b28d-solo-PvP_Room] ORIG 0x085d8b2a `call; test %al,%al; je` 直测 + then
    // 分支（mode!=4&&!=5 → 1→4/2→5）直落、else 分支（4→1/5→2）后置；去 cVar1
    // 局部使 out 参数槽对齐 -0x9。
    if (IsDeathMatchMap(mapNo, local_d))
    {
        if ((m_pvpBattleMode != 4) && (m_pvpBattleMode != 5))
        {
            if (m_pvpBattleMode == 1)
            {
                m_pvpBattleMode = 4;
            }
            else if (m_pvpBattleMode == 2)
            {
                m_pvpBattleMode = 5;
            }
        }
    }
    else
    {
        if (m_pvpBattleMode == 4)
        {
            m_pvpBattleMode = 1;
        }
        else if (m_pvpBattleMode == 5)
        {
            m_pvpBattleMode = 2;
        }
    }
}

// [b28d-solo-PvP_Room] AE: P248 档①/P292 已落 —— IsDeathMatchMap bool 化后调用点
// `call; mov %al,-0x9` 直存与 ORIG 0x085dd936 全等（旧「test;setne 归一封顶」判词作废）。
bool PvP_Room::CheckPvPMapMode(CUser* user, int mapNo, bool& flag)
{
    if (m_pvpBattleMode == 6)
    {
        return 1;
    }
    // ORIG 085dd9xx：bool 型 cVar1（call 结果 al 直存 byte 槽、无归一）；
    // !cVar1 判定走 movzbl+xor $1 的 4.1 取反形。
    bool cVar1 = IsDeathMatchMap(mapNo, flag);   // bool 形：后两处 !cVar1 的 movzbl+xor 物化与 ORIG 同；
    // 差仅入口 mov%al 直存 vs 4.4 test;setne 归一（N bool ABI），净差 -2 优于 uchar 形 +4，
    // char 形实测 ndiff 12（!cVar1 折成 cmpb/jne，丢 movzbl+xor+je 四段×2）
    if ((cVar1 != 0) && (!flag) && (m_pvpBattleMode == 4))
    {
        return 1;
    }
    if ((cVar1 != 0) && (m_pvpBattleMode == 3))
    {
        m_pvpBattleMode = 4;
    }
    if ((cVar1 != 0) && (m_pvpBattleMode == 1))
    {
        set_pvp_mode(user, 4);
    }
    if ((cVar1 != 0) && (m_pvpBattleMode == 2))
    {
        m_pvpBattleMode = 5;
    }
    // ORIG 085dd9ef/085dda0x：!char-slot 形 = movzbl -0x9 + xor $1 + test（xor
    // 值化，非 cmp 归一）；(cVar1^1) 写法被 4.4 fold-const 折成 cmpb $1。
    if ((!cVar1) && (m_pvpBattleMode == 4))
    {
        set_pvp_mode(user, 1);
    }
    if ((!cVar1) && (m_pvpBattleMode == 5))
    {
        m_pvpBattleMode = 2;
    }
    return 1;
}

int PvP_Room::set_pvp_mode(CUser* user, int mode)
{
    if (m_state != 1)
    {
        return 0x13;
    }
    if (m_seats[7] != user)
    {
        return 8;
    }
    lock();
    int iVar1 = m_pvpBattleMode;
    // ORIG 085d84ee: arg 先载（%edx）再 this（%eax）写回。N76 异型化翻案
    // （NL7/NL8 新定律）：m_pvpBattleMode int→unsigned int（private，正数
    // == 比较，无符号性语义差），4.4 异型 store 出实参先载形，ndiff 2→0。
    m_pvpBattleMode = mode;
    if (mode == 1)
    {
        for (int local_30 = 0; local_30 < 8; local_30 = local_30 + 1)
        {
            if (P_SEATS(local_30) != 0)
            {
                P_TEAMS(local_30) = 0;
                m_relayMgr.OnChangeTeam(local_30, 0);
            }
        }
    }
    else if (mode == 3)
    {
        if (iVar1 == 2)
        {
            for (int local_2c = 0; local_2c < 8; local_2c = local_2c + 1)
            {
                if (P_SEATS(local_2c) != 0)
                {
                    m_relayMgr.OnChangeTeam(local_2c, P_TEAMS(local_2c));
                }
            }
        }
        else if (iVar1 == 1)
        {
            int local_28 = 1;
            for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
            {
                if (P_SEATS(local_24) != 0)
                {
                    P_TEAMS(local_24) = local_28;
                    m_relayMgr.OnChangeTeam(local_24, P_TEAMS(local_24));
                    if (local_28 == 1)
                    {
                        local_28 = 2;
                    }
                    else
                    {
                        local_28 = 1;
                    }
                }
            }
        }
    }
    else if (mode == 4)
    {
        for (int local_20 = 0; local_20 < 8; local_20 = local_20 + 1)
        {
            if (P_SEATS(local_20) != 0)
            {
                P_TEAMS(local_20) = 0;
            }
        }
    }
    else if (mode == 5)
    {
        int local_1c = 1;
        for (int local_18 = 0; local_18 < 8; local_18 = local_18 + 1)
        {
            if (P_SEATS(local_18) != 0)
            {
                P_TEAMS(local_18) = local_1c;
                m_relayMgr.OnChangeTeam(local_18, local_1c);
                if (local_1c == 1)
                {
                    local_1c = 2;
                }
                else
                {
                    local_1c = 1;
                }
            }
        }
    }
    else if (iVar1 != 3)
    {
        int local_14 = 1;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) != 0)
            {
                P_TEAMS(local_10) = local_14;
                m_relayMgr.OnChangeTeam(local_10, P_TEAMS(local_10));
                if (local_14 == 1)
                {
                    local_14 = 2;
                }
                else
                {
                    local_14 = 1;
                }
            }
        }
    }
    unlock();
    return 0;
}

// [b28d-solo-PvP_Room] AE 归零（76=76，本轮）：三处新形组合 ——
// ①P248 档②：PVPMAP_ONLY_* 数组元素改 bool（ORIG 对 `!arr[i]` 发
//   `movzbl; xor $1; test; je/jne`；char 元素时 4.4 恒丢 xor）；
// ②循环改 for 形（增量入第三式、CAND_MAP_NEXT 空标签）⇒ 4.4 循环头/尾直落
//   call，消 while 形的两处中继 jmp；
// ③`if ((mode!=1)&&(mode!=4)) { } else if (!local_d) goto NEXT;` 的
//   fall-through 空 then 取代 `goto CAND_MAP_PUSH` ⇒ 消向前 goto 的 `jmp <nop+1>`。
// 【语义核对】local_d==0 才跳过（等价原短路项 `(mode==1||mode==4) && local_d` 为假）。
int PvP_Room::GetCandidateMapIndexList(std::vector<int>& out)
{
    bool local_d;
    local_d = false;
    // [b28d-solo-PvP_Room] 试 for 形：ORIG 0x085d87xx 循环头/尾各直落 call
    // （4.4 while 形多中继 jmp）；增量入 for 第三式，CAND_MAP_NEXT 为空标签。
    int local_14;
    for (local_14 = 1;
         (local_14 < G_CDataManager()->m_mapList->GetPVPMapCount() + 1) && (local_14 < 0x14);
         local_14 = local_14 + 1)
    {
        // [b28d-solo-PvP_Room] P222/P223：ORIG 0x085d87e6 `call; test %al; je PUSH`
        // （bool callee 正极性直判）+ mode==3 / local_d 两独立岛（0xa7/0xaa）；
        // 短路 `==0 ||` 形在 4.4 反产 xor 物化 + 汇合岛。
        if (IsDeathMatchMap(local_14, local_d))
        {
            if (m_pvpBattleMode == 3)
                goto CAND_MAP_NEXT;
            if ((m_pvpBattleMode != 1) && (m_pvpBattleMode != 4))
            {
                // fall through 到 PUSH（删 `goto CAND_MAP_PUSH`：4.4 为向前 goto
                // 紧邻标签恒发 `jmp <nop+1>` 中继，ORIG 无）。
            }
            else if (!local_d)
            {
                // [b28d-solo-PvP_Room] 语义核对：ORIG `movzbl local_d; xor $1; test; jne NEXT`
                // ⇒ local_d==0 才跳过（等价原短路项 `(mode==1||mode==4) && local_d` 为假）。
                goto CAND_MAP_NEXT;
            }
        }
    CAND_MAP_PUSH:
        // [b28d-7] P167 EXTRA 修正：ORIG 0x085d885e 只有**一个**共享
        // push_back 调用点（两分支 goto 汇入），独立两语句多 1 个调用点。
        // [b28d-solo-PvP_Room] ORIG 0x085d885x：channel==7 分支尾直落 push_back
        // （无 `jmp PUSH_BACK`）；改 if/else 两臂各 `goto NEXT` 后直落，消多余 jmp。
        if (G_GameWorld()->GetChannelType() == 7)
        {
            if (!PVPMAP_ONLY_TOURNAMENT_RANDOM[local_14])
                goto CAND_MAP_NEXT;
        }
        else
        {
            if (!PVPMAP_ONLY_NORMAL_EXPOSURE[local_14])
                goto CAND_MAP_NEXT;
        }
    CAND_MAP_PUSH_BACK:
        out.push_back(local_14);
    CAND_MAP_NEXT:
        ;
    }
}

// [b28d-7 N7] ORIG 0x085dc340：gen_timer_key→%ebx / m_index→%esi（0x085dc36b/085dc375），4.4 反配（同 VoteLackUser 判例）；裸 register 实测无效 ⇒ N7 封顶。
// [b28d-solo-PvP_Room] N7-N109: ORIG 0x085dc340 key->ebx/m_index->esi，4.4 反配
int PvP_Room::insert_timer_recv_pvp_rank()
{
    // 残余 4 行 N7 互换：gen_timer_key 结果与 m_index 的 callee-saved 缓存
    // ORIG 用 ebx/esi、4.4 反序（同 VoteLackUser 判例），封顶。
    if (m_isInsertTimerRecvPvpRank != 0)
    {
        return 1;
    }
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)1, m_index, (TIMER_MESSAGE)0x3c, 10000LL,
        (int)gen_timer_key((TIMER_MESSAGE)0x3c), 0);
    m_isInsertTimerRecvPvpRank = 1;
    return 1;
}

// [b28d-solo-PvP_Room] P0 具名局部: ORIG key->edi + m_index 落 -0x1c 槽（P0 槽往返证据），语句表达式形已复现该分配；残余 idx 槽写时机（4.1 延后物化）封顶
// [b28d-solo-PvP_Room] 本轮复测（N7/N18a 封顶）：ORIG key→%edi + m_index 落 -0x1c；
// 4.4 恒把首个 callee 结果（key）spill 到 -0x2c 并把 m_index 经 -0x1c 升格 %edi
// （角色互换）。五形实测：内联双参 43 条（key 落 -0x1c、m_index→edi，角色仍反）、
// m_index 语句表达式 + 内联 key 45 条（取最优，槽位与 ORIG 同集合）、P197② 具名
// key 47 条、裸语句表达式 key 45 条、register 提示 46 条 ⇒ 不可翻，封顶。
int PvP_Room::insert_timer_recv_pvp_end_result()
{
    // [b28d-solo-PvP_Room P0/P226] ORIG 0x085dc23c：gen_timer_key→%edi、m_index
    // 经 -0x1c 槽 store/reload（P0 判据：槽往返 ⇒ 原码有具名局部/语句表达式临时）。
    // 语句表达式 `({ int idx = m_index; idx; })` 复现 key→%edi + m_index 落 -0x1c
    // 槽；残余仅 idx 槽写时机（4.1 延后到 timeout 求值后，4.4 提前到语句头）。
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)1, ({ int idx = m_index; idx; }), (TIMER_MESSAGE)0x3d,
        ((IMatch*)m_matching)->GetEndPvPResultTimeout(),
        (int)gen_timer_key((TIMER_MESSAGE)0x3d), 0);
    return 1;
}

int PvP_Room::check_recv_pvp_rank()
{
    // ORIG 0x085dc3ca：bool 槽 -0xd（movzbl 返回）；user 直落 %ebx 跨调用；
    // IsRecv(char) 的 ! 以 xor $1/test/je 短路，不物化 0/1。
    unsigned char changed = 0;
    for (int i = 0; i < 8; i = i + 1)
    {
        // ORIG 085dc3ca：4.1 双空 skip 块岛 085dc455(nop;jmp end)/085dc458(nop;fall end)。
        // goto 落点保留独立块（4.4 直写 continue 会被 threading 折叠）。
        if (P_SEATS(i) == 0)
            goto SKIP_RANK_SEAT;
        if (P_TEAMS(i) == 0xff)
            goto SKIP_RANK_TEAM;
        if (!m_recvFlagB4.IsRecv(i))
        {
            m_fieldC0 = 0;
            register CUser* user = P_SEATS(i);
            G_CGameManager()->CheckOutPvp(user, true);
            changed = 1;
        }
SKIP_RANK_TEAM:
        ;
SKIP_RANK_SEAT:
        ;
    }
    return changed;
}

// [b28d-7 N95/N82] ORIG 0x085da5f4：连续槽清零组 -0x20..-0x50，4.4 槽组整体深移 0x18（-0x2c..-0x54）+ 帧差；P167 CLEAR ⇒ 槽分配策略封顶。
// [b28d-solo-PvP_Room] 本轮：P225 推广 —— else 分支 `(p!=0)&&(k=guildkey(),k!=0)` 拆嵌套 if，
// 消 `mov$1/jmp/mov$0/test;je` bool 物化岛（139→134=ORIG）；残余 N82/N4 槽号 + scaled 折叠
// （ORIG `mov -0x40(%ebp,%ebx,4)` vs 4.4 `lea 0x4(%eax),%ebx` + scaled）。
// [SweepPvPRoom] 复核 gap=0（134=134）：ORIG 的 `-0x40/-0x50(%ebp,%ebx,4)` 是同一
// local_54[8] 数组的 `[i+4]`/`[i]` disp 折叠（Ghidra 同证单数组）；4.4 用
// `lea 0x4(%eax),%ebx` 再 scaled ⇒ N4/P30 伪影。另 ORIG 首初始化 `movl $0x4,-0x2c`
// 为死局部（Ghidra local_30=4，后续未读，P171/P232 族）⇒ 无源码解，封顶。
bool PvP_Room::IsThereSameGuildMember()
{
    if ((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3))
    {
        int local_dead;
        local_dead = 4;
        (void)local_dead;
        int local_54[8];
        local_54[4] = 0;
        local_54[5] = 0;
        local_54[6] = 0;
        local_54[7] = 0;
        local_54[0] = 0;
        local_54[1] = 0;
        local_54[2] = 0;
        local_54[3] = 0;
        int local_2c = 0;
        int local_28 = 0;
        for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
        {
            if (P_TEAMS(local_24) == 1)
            {
                CUser* local_34 = P_SEATS(local_24);
                if (local_34 != 0)
                {
                    local_54[local_2c + 4] = local_34->get_charac_guildkey();
                    local_2c = local_2c + 1;
                }
            }
            else if (P_TEAMS(local_24) == 2)
            {
                CUser* local_34 = P_SEATS(local_24);
                if (local_34 != 0)
                {
                    local_54[local_28] = local_34->get_charac_guildkey();
                    local_28 = local_28 + 1;
                }
            }
        }
        if ((local_2c == 0) || (local_28 == 0))
        {
            return 0;
        }
        for (int local_20 = 0; local_20 < local_2c; local_20 = local_20 + 1)
        {
            for (int local_1c = 0; local_1c < local_28; local_1c = local_1c + 1)
            {
                if (local_54[local_20 + 4] == local_54[local_1c])
                {
                    return 1;
                }
            }
        }
    }
    else
    {
        int local_18 = 0;
        int local_14;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            CUser* local_34 = P_SEATS(local_10);
            // [b28d-solo-PvP_Room] P225 推广：ORIG 0x085da5f4 为两段独立
            // `cmpl $0; je <next>` 直跳（无 setcc/bool 物化岛）；`&&`+逗号表达式
            // 在 4.4 恒物化成 `mov $1; jmp; mov $0; test %al; je`（+5 条）⇒ 拆嵌套 if。
            if (local_34 != 0)
            {
                local_14 = local_34->get_charac_guildkey();
                if (local_14 != 0)
                {
                    if (local_18 == local_14)
                        return 1;
                    local_18 = local_14;
                }
            }
        }
    }
    return 0;
}

int PvP_Room::get_extra_win_point(int team)
{
    // N7 封顶（虚调用 callee 寄存器互换，见 get_user_list 注释）。
    return ((IMatch*)m_matching)->GetExtraWinPoint(team);
}

int PvP_Room::battle_reset()
{
    lock();
    for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
    {
        P_WIN(local_14) = 0;
        P_TEAMWIN(local_14) = 0;
        P_SEATSTATE(local_14) = 0;
        if (P_SEATS(local_14) != 0)
        {
            P_SEATS(local_14)->getHackAnalyzer()->reportHackInfo();
            P_SEATS(local_14)->getHackAnalyzer()->resetHackInfo();
        }
    }
    memset(m_field5dc, 0, 0xc);
    m_field5d8 = 0;
    m_state = 1;
    m_recvFlagB4.Clear();
    m_recvFlagBC.Clear();
    m_userTable.Clear();
    m_fieldC0 = 0;
    m_relayMgr.Reset();
    m_battleEndFlag = 0;
    m_rankRequested = 0;
    m_isInsertTimerRecvPvpRank = 0;
    m_isEndPvpBattle = 0;
    for (int local_10 = 0x31; local_10 < 0x3f; local_10 = local_10 + 1)
    {
        // ORIG 085d657d: cmpl $0x3e / je 085d6597(空块 nop) / call / jmp 085d6598
        // 4.1 保留的空 skip 块落点在 call 之后、++ 之前，用 goto 钉住块序。
        if (local_10 == 0x3e)
            goto SKIP_GEN_TIMER_KEY;
        gen_timer_key((TIMER_MESSAGE)local_10);
SKIP_GEN_TIMER_KEY:
        ;
    }
    unlock();
}

int PvP_Room::reset()
{
    battle_reset();
    lock();
    m_joinCount = 0;
    m_state = 0;
    m_pvpBattleMode = 2;
    m_mode8 = 0;
    memset(m_roomName, 0, 0x1e);
    m_seats[7] = 0;
    m_managerSeat = 0;
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        P_SEATS(local_10) = 0;
        P_TEAMS(local_10) = 0xff;
        P_SEATSTATE(local_10) = 0;
    }
    if (G_GameWorld()->GetChannelType() == 6)
    {
        m_guildWarLog.Clear();
    }
    m_relayMgr.Reset();
    m_relayMgr.SetParent(this);
    m_deathMatch.Reset();
    m_deathMatch.SetParent(this);
    m_battleEndFlag = 0;
    m_rankRequested = 0;
    m_isInsertTimerRecvPvpRank = 0;
    m_isEndPvpBattle = 0;
    unlock();
}

int PvP_Room::calculate_pvp_point()
{
    m_userTable.Clear();
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        // ORIG 085d9090：call 后 test %al;jne（无 xor/归一）；if/else 两支尾
        // + observer 空 then 共三条 jmp merge（@41/@56/@57）。
        if (P_SEATS(local_10) == 0)
            goto CP_NEXT;
        if (IsPvpObserver(local_10))
        {
            goto CP_NEXT;
        }
        if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
        {
            m_userTable.Set(local_10, P_SEATS(local_10));
            goto CP_NEXT;
        }
        else
        {
            m_userTable.Set(P_TEAMWIN(local_10), P_SEATS(local_10));
            goto CP_NEXT;
        }
CP_NEXT:
        ;
    }
    m_userTable.Calculate();
}

int PvP_Room::update_pvp_point()
{
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        if (G_GameWorld()->IsFreePvPChannel() != 0)
        {
            for (int local_18 = 0; local_18 < 8; local_18 = local_18 + 1)
            {
                if (P_SEATS(local_18) == 0)
                    continue;
                if (IsPvpObserver(local_18) != false)
                    continue;
                P_SEATS(local_18)->update_old_pvp_point();
            }
            goto LAB_085dca42;
        }
    }
    for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
    {
        if (P_SEATS(local_14) == 0)
            continue;
        if (IsPvpObserver(local_14) != false)
            continue;
        {
            P_SEATS(local_14)->update_old_pvp_point();
            if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
            {
                P_SEATS(local_14)->update_pvp_point(
                    m_userTable.GetResult(local_14, false,
                                          P_SEATS(local_14)->get_pvp_grade()));
            }
            else
            {
                bool mid = false;                       // ORIG -0x9 byte 槽
                if (P_SEATS(local_14)->get_pvp_grade() > 10)
                    mid = true;
                P_SEATS(local_14)->update_pvp_point(
                    m_userTable.GetResult(P_TEAMWIN(local_14), mid,
                                          P_SEATS(local_14)->get_pvp_grade()));
            }
        }
    }
LAB_085dca42:
    ;
}

// [b28d-7 N82附] ORIG 0x085d8c3a：成员值经 callee-saved 跨 cMyTrace 构造保活（movzwl/movswl 两段），4.4 改槽中转（P227/P0 边界）；373/373 助记符全等、P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] 本轮：P227 三处 cMyTrace 改全表达式临时链（vararg 在 ctor 前求值升格
// callee-saved）、P46 switch 叶块序改 1/4→3→2/5、P59 三 case 两体交换（jle 直落 else）；
// 残余仅 P75/N7 实参装载时刻（local_14→%ebx 跨 receiver call、GetPVPMapCount 求值序）⇒ 封顶。
void PvP_Room::start_pvp()   // [b28c-16 SIGFIX] ORIG 串 void
{
    if (get_waiter_count() < 2)
    {
        goto LAB_085d91da;
    }
    {
        ((IMatch*)m_matching)->RequestMap(m_requestedMapNo,
                                          reinterpret_cast<PVP_BATTLE_MODE&>(m_pvpBattleMode));
        if ((m_requestedMapNo < 0) || (m_requestedMapNo > G_CDataManager()->m_mapList->GetPVPMapCount()))
        {
            // [b28d-solo-PvP_Room] P227：ORIG 0x085d8c3a 在 ctor call 之前
            // 右起求 vararg 并跨 ctor 保活 callee-saved（`movswl %ax,%ebx` 先于
            // ctor，`mov %ebx,0x8(%esp)` 后于 ctor）⇒ 全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x880, 0)(
                "START_PVP request_map_index(%d)", (int)m_requestedMapNo);
            m_requestedMapNo = 0;
        }
        if (m_requestedMapNo == 0)
        {
            m_playMapNo = (short)SelectRandomMap();
            if ((m_playMapNo < 0) ||
                (m_playMapNo >= G_CDataManager()->m_mapList->GetPVPMapCount()))
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x88b, 0)(
                    "START_PVP select_map_index(%d)", (int)m_playMapNo);
            }
        }
        else
        {
            m_playMapNo = m_requestedMapNo;
        }
        if (G_GameWorld()->GetChannelType() == 6)
        {
            m_guildWarLog.SetpvpStartTime(GlobalData::s_systemTime_.getCurSec());
        }
        int local_18 = 0;
        PacketGuard local_54;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) != 0)
            {
                local_18 = local_18 + 1;
                P_WIN(local_10) = 1;
                P_TEAMWIN(local_10) = (unsigned char)P_TEAMS(local_10);
                m_relayMgr.OnChangeTeam(local_10, (int)P_TEAMWIN(local_10));
                P_SEATS(local_10)->reset_pvp_masterid_walkingout_me();
                local_54.clear();
                local_54.put_header(0, 0x2d);
                local_54.put_byte((int)m_playMapNo);
                local_54.put_byte(m_pvpBattleMode);
                local_54.finalize(true);
                P_SEATS(local_10)->Send(local_54);
                P_SEATS(local_10)->set_state((ch_state)7);
                P_SEATS(local_10)->getHackAnalyzer()->beginCollectHackInfo(P_SEATS(local_10));
                disconnect_detecter::GetInstanceDisconnectDetecter()->RegisterUser(
                    P_SEATS(local_10));
            }
        }
        Statistics::PVPPlay2((int)m_playMapNo, m_pvpBattleMode, local_18);
        calculate_pvp_point();
        m_fieldBc = get_waiter_count();
        m_state = 2;
        if (m_pvpBattleMode == 3)
        {
            m_relayMgr.OnStart();
        }
        if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
        {
            m_deathMatch.OnStart();
        }
        m_battleStartSec = GlobalData::s_systemTime_.getCurSec();
        int local_14 = 0;
        switch (m_pvpBattleMode)
        {
        case 1:
        case 4:
            // [b28d-solo-PvP_Room] P59 双向布局：ORIG 0x085d8c3a 为 `cmpl $2; jle <T>`，
            // then 体（local_14=0）落 <T>、else 体（=1）直落 ⇒ 条件取反+交换两体。
            // P46 叶块顺序：ORIG 0x85d9080(1/4)→0x85d9098(3)→0x85d90b0(2/5) ⇒ case 3 提前。
            if (local_18 >= 3)
            {
                local_14 = 1;
            }
            else
            {
                local_14 = 0;
            }
            break;
        case 3:
            if (local_18 >= 3)
            {
                local_14 = 5;
            }
            else
            {
                local_14 = 4;
            }
            break;
        case 2:
        case 5:
            if (local_18 >= 3)
            {
                local_14 = 3;
            }
            else
            {
                local_14 = 2;
            }
            break;
        }
        // [b28d-solo-PvP_Room] P75/N7 封顶：ORIG 在 receiver call 前把 local_14 升格
        // %ebx 保活（`mov -0x10(%ebp),%ebx; call; mov %ebx,0x8(%esp)`）；P226 裸语句
        // 表达式实参实测无效（具名变量读取不升格）⇒ 跨版本装载时刻差。
        GetInstanceCommonStatisticsMgr()->IncreaseQuantity(
            (CCommonStatisticsMgr::ENUM_STATISTIC_CONTAINER)1, local_14);
        if ((m_playMapNo >= 0) &&
            (m_playMapNo < G_CDataManager()->m_mapList->GetPVPMapCount()))
        {
            GetInstanceCommonStatisticsMgr()->IncreaseQuantity((CCommonStatisticsMgr::ENUM_STATISTIC_CONTAINER)2, (int)m_playMapNo);
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x942, 0)(
                "START_PVP filter select_map_index(%d)", (int)m_playMapNo);
        }
        ((IMatch*)m_matching)->OnStartFight();
    }
LAB_085d91da:
    ;
}

// [b28d-7 N82附] ORIG 0x085d9386：4 个 getter 结果先物化 %edi/%esi/%ebx 跨 cMyTrace 构造保活，4.4 改槽中转；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] N82附: ORIG 0x085d9386 4 getter 结果物化 %edi/%esi/%ebx 跨 cMyTrace 构造保活
// [b28d-solo-PvP_Room] 形参序修复（真语义 bug）：ORIG 0x085d9386 用 param_1
// (0xc(%ebp)) 做 get_user_seat/P_WIN 检查/日志（= victim）、param_2 做
// IncreaseKillCount/Update_Kill_event（= killer）；调用点 ORIG 0x081ecf47
// `die_charac(dispatch_sig 的 user, target)` 第一实参即死者。原实现按
// (killer, victim) 声明 ⇒ 与 ORIG 位置相反（旧 DIFF 主因）。交换形参名后
// 函数体零改动、调用点零改动，机器码与 ORIG 对齐。
// [b28d-solo-PvP_Room] 本轮复测（P167 CLEAR 36/36）：日志段按 P87 register 形
// 预求值（get_pvp_battle_mode→槽、get_index→%edi、getCurCharacName→%esi、
// get_acc_name→%ebx，跨 CSwitchLog ctor 保活）后，0xc/0x10 形参错位与 ctor
// 前求值序全部归位；残余仅槽号（ORIG local_5c@-0x5c / CSwitchLog@-0x3c vs
// 4.4 -0x1c / -0x44）⇒ N82 槽分配策略封顶。
// [b28d-solo-PvP_Room] 本轮真语义 bug 修复：check_winner 调用点两分支体整体反置 ——
// ORIG 0x085d961b `call; test %al,%al; je 0x085d966d`：al==0 落 fall-through
// （OnDiePlayer(...,0)+pvp_request_pvp_rank+return -1），al!=0 落 0x085d966d
// （OnDiePlayer(...,m_pvpBattleMode==3)+return 0）⇒ `==0` 改 `!=0`（消 xor 物化 + 语义归位）。
int PvP_Room::die_charac(CUser* victim, CUser* killer)
{
    int mode;
    int local_30;
    int local_2c;
    int local_28;
    int local_24;
    int local_20;
    // [b28d-solo-PvP_Room] P87 register 形：ORIG 0x085d9386 在 CSwitchLog ctor
    // 之前按 get_pvp_battle_mode()→槽 -0x5c、get_index()→%edi、getCurCharacName()
    // →%esi、get_acc_name()→%ebx 顺序求值（跨 ctor 保活），varargs 按
    // acc/name/index/mode 压栈。
    int local_5c = get_pvp_battle_mode();
    register int local_58 = get_index();
    register const char* local_54 = victim->getCurCharacName();
    register const char* local_50 = victim->get_acc_name();
    {
        // [b28d-solo-PvP_Room] P227：ORIG 0x085d9386 的 CSwitchLog 为全表达式临时链
        // （ctor 前后槽/寄存器装载与 ORIG 同向）。
        CSwitchLog(__PRETTY_FUNCTION__, 0x9a1, 0, 0)(   // ORIG 同文（P139）
            "pvp@log %s,%s,%d,%d", local_50, local_54, local_58, local_5c);
    }
    if (m_state != 2)
    {
        return 0x13;
    }
    lock();
    local_30 = get_user_seat(victim);
    if (local_30 < 0)
    {
        unlock();
        return 0xffffffff;
    }
    if (P_WIN(local_30) != 1)
    {
        unlock();
        return 0x12;
    }
    P_WIN(local_30) = 0;
    unlock();
    PacketGuard local_4c;
    local_4c.put_header(0, 0x2e);
    local_4c.put_byte(local_30);
    // [b28d-t3-PoolBigThree] P59 块序：ORIG 0x085d95e? `test; je <L_m1>` 后
    // **fall-through 即 get_user_seat 分支**（-1 岛后置 @0x085d9605），旧 `==0`
    // 形把 -1 分支放在 fall-through ⇒ 整体与 ORIG 错位 8 行。改极性正向。
    // （本件台账 74→本轮起点 113（solo 形参序真修后布局重排）→**93**；残余 =
    // N82 槽号平移（-0x5c↔-0x1c 族）+ P75 装载时刻 + C2/C1 显示差。）
    if (killer != 0)
    {
        local_4c.put_byte(get_user_seat(killer));
    }
    else
    {
        local_4c.put_byte(-1);
    }
    local_4c.finalize(true);
    send_to_pvp(local_4c);
    if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
    {
        local_2c = get_user_seat(killer);
        m_deathMatch.IncreaseKillCount(local_2c);
        m_deathMatch.IncreaseDeathCount(local_30);
        m_deathMatch.InsertTimerKilledUserForRevive(get_index(), local_30);
    }
    if (killer != 0)
    {
        local_28 = (int)killer->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
        MissionClearCondition_Parameter local_5b((PVP_BATTLE_MODE)m_pvpBattleMode);
        ((CMissionList_Charac*)local_28)->Update_Kill_event(*killer, local_5b);
    }
    // [b28d-solo-PvP_Room] 真语义 bug 修正（分支体整体反置）：ORIG 0x085d961b
    // `call check_winner; test %al,%al; je 0x085d966d` —— al==0 落 fall-through
    // （OnDiePlayer(...,0)+pvp_request_pvp_rank+return -1），al!=0 落 0x085d966d
    // （OnDiePlayer(...,m_pvpBattleMode==3)+return 0）。旧形 `==0` 把两分支体互换。
    if (check_winner() != 0)
    {
        local_20 = get_user_seat(killer);
        m_relayMgr.OnDiePlayer(local_30, local_20, m_pvpBattleMode == 3);
        return 0;
    }
    local_24 = get_user_seat(killer);
    m_relayMgr.OnDiePlayer(local_30, local_24, false);
    pvp_request_pvp_rank();
    return 0xffffffff;
}

// [b28d-7 N82] ORIG 0x085d9b0e：槽 -0x48 与计数槽，4.4 -0x4c/-0x44（槽基差 4 + 帧 365/373）；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] N82: ORIG 0x085d9b0e 槽 -0x48 与计数槽 vs 4.4 -0x4c/-0x44；P167 CLEAR
int PvP_Room::time_out(int* seats)
{
    // ORIG 0x085d9b0e（365 条；本侧 373，+8 全为 N 类形态差，台账豁免）：
    //  - local_4c/local_3d 的 ==1/!=0 检查 4.1 折成 `xor$1,%eax;test%al;je`
    //    常量岛（4.4 恒 `cmpb/cmpl` 直判，setcc-fold 负例在册）；
    //  - PVP_*_BATTLE_TIME[local_4c] 与 local_84[teamwin+5] 索引 4.1 单段
    //    `mov &arr(,%eax,4)`/`lea …,%edx` 直落，4.4 拆 lea+reload（N76/N18a）；
    //  - end_time 日志 3 实参 4.1 以 callee-saved 缓存（push 对），4.4 逐槽
    //    reload（+4 mov）。lock()/unlock() 调用点、模式分派、do-while 比较
    //    （jl/jle 方向）已逐段核对无语义缺口。
    // [b28d-t3-PoolBigThree] **148→84**（本轮起点 148=官方 s408 现值；台账 330 为
    // b28d 期快照）三修：①模式分派 if 链→**switch((int)mode)**（ORIG 0x085d9b81
    // `cmp $1; jl; cmp $2; jle; sub $4; cmp $1; ja` 有符号直跳链实证，(int) 收窄复现
    // jl/jle 非 jb/jbe）；②日志条件 `(x!=y)&&(x-y>=0)` 双条件→ `local_44 > local_48`
    // （ORIG 0x085d9bf3 `test; jle` 单测，旧形产 js 域差岛）；③local_3d int→**bool**
    // （ORIG 0x085d9c51 movb 直存 + 0x085d9cb5 movzbl/xor$1 取反物化，P248 档②）。
    // 残余 84 = 4.1 sub+test 物化（0x085d9be5，4.4 恒 cmp 折叠）+ N76/N18a scaled
    // 折叠 + N82 槽号 + C2/C1/重定位显示差。insn 365/367。
    if (m_battleEndFlag != 0)
    {
        return 0;
    }
    m_battleEndFlag = 1;
    int local_4c = 0;
    if (G_GameWorld()->GetChannelType() == 7)
    {
        local_4c = 1;
    }
    int local_48 = GlobalData::s_systemTime_.getCurSec() - m_battleStartSec;
    int local_44 = 0;
    // [b28d-t3-PoolBigThree] ORIG 0x085d9b81..9b99 直跳链 `cmp $1; jl <def>;
    // cmp $2; jle <c12>; sub $4; cmp $1; ja <def>` = **switch(case 1,2 / 4,5)**
    // 4.1 比较链展开（旧 if 链形 4.4 产 `cmpl $0;jle; cmpl $2;jg` 极性与指令
    // 数均不合）；日志条件 ORIG 0x085d9be5 `sub; test %eax; jle <skip>` 单测
    // ⇒ 合并 `!= && >=0` 双条件为 `>`（原形 js 与 ORIG jle 在 ==0 值域分叉，
    // 真语义修正候选：ORIG 对 limit==elapsed 走 skip，旧形经 && 折叠同结果，
    // 但物化形 js 域差 -2^31，见判词）。（int) 收窄比较域 ⇒ 复现 ORIG 有符号
    // jl/jle（unsigned 形 4.4 恒 jb/jbe；值域 0..5 下转换不改语义）。
    switch ((int)m_pvpBattleMode)
    {
    case 1:
    case 2:
        local_44 = PVP_NORMAL_BATTLE_TIME[local_4c] / 1000;
        break;
    case 4:
    case 5:
        local_44 = PVP_DM_BATTLE_TIME[local_4c] / 1000;
        break;
    }
    if (local_44 > local_48)
    {
        cMyTrace local_5c(__PRETTY_FUNCTION__, 0xa9a, 0);
        local_5c("PVP_TIMEOUT_CHECK end_time(%d), play_time(%d)", local_44, local_48);
    }
    if (m_state != 2)
    {
        return 0x13;
    }
    // [b28d-t3-PoolBigThree] P248 档②：ORIG 0x085d9c51/0x085d9c7b `movb $1/$0,-0x39`
    // + 0x085d9cb5 `movzbl; xor $1; test; je` ⇒ bool 局部（int 形反产 movl+cmpl/jne）。
    bool local_3d = true;
    for (int local_3c = 0; local_3c < 7; local_3c = local_3c + 1)
    {
        if (seats[local_3c] != seats[local_3c + 1])
        {
            local_3d = false;
            break;
        }
    }
    if ((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3))
    {
        lock();
        if (!local_3d)
        {
            int local_84[10];
            local_84[5] = 0;
            local_84[6] = 0;
            local_84[7] = 0;
            local_84[8] = 0;
            local_84[9] = 0;
            local_84[0] = 0;
            local_84[1] = 0;
            local_84[2] = 0;
            local_84[3] = 0;
            local_84[4] = 0;
            for (int local_30 = 0; local_30 < 8; local_30 = local_30 + 1)
            {
                if ((P_SEATS(local_30) != 0) && (P_TEAMWIN(local_30) != 0) &&
                    (P_TEAMWIN(local_30) < 0xfe))
                {
                    local_84[P_TEAMWIN(local_30) + 5] = local_84[P_TEAMWIN(local_30) + 5] + 1;
                    local_84[P_TEAMWIN(local_30)] =
                        local_84[P_TEAMWIN(local_30)] + seats[local_30];
                }
            }
            unsigned int local_38 = 0;
            int local_34 = 0x7fffffff;
            for (unsigned int local_2c = 1; (int)local_2c < 5; local_2c = local_2c + 1)
            {
                if ((0 < local_84[local_2c + 5]) && (local_84[local_2c] < local_34))
                {
                    local_38 = local_2c;
                    local_34 = local_84[local_2c];
                }
            }
            for (int local_28 = 0; local_28 < 8; local_28 = local_28 + 1)
            {
                if (P_TEAMWIN(local_28) == local_38)
                {
                    P_WIN(local_28) = 1;
                }
                else
                {
                    P_WIN(local_28) = 0;
                }
            }
            m_userTable.SetWinTeam(local_38);
        }
        else
        {
            for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
            {
                P_WIN(local_24) = 0;
            }
        }
        unlock();
    }
    else if (m_pvpBattleMode == 5)
    {
        m_deathMatch.checkWinnerForTeamPlay(m_users, m_winCheck, m_teamWin, m_userTable);
    }
    else if (m_pvpBattleMode == 4)
    {
        m_deathMatch.checkWinnerForSinglePlay(m_users, m_winCheck, m_userTable);
    }
    else
    {
        lock();
        if (local_3d == 1)
        {
            for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
            {
                P_WIN(local_10) = 0;
            }
        }
        else
        {
            int local_20 = 0;
            int local_1c = 0x7fffffff;
            for (int local_18 = 0; local_18 < 8; local_18 = local_18 + 1)
            {
                if ((P_SEATS(local_18) != 0) && (seats[local_18] < local_1c))
                {
                    local_20 = local_18;
                    local_1c = seats[local_18];
                }
            }
            for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
            {
                if (local_20 == local_14)
                {
                    P_WIN(local_14) = 1;
                }
                else
                {
                    P_WIN(local_14) = 0;
                }
            }
            m_userTable.SetWinTeam(local_20);
        }
        unlock();
    }
    pvp_request_pvp_rank();
    return 0;
}

// §6.2 巨件核销：P167 153/153 [0缺0多]；gap 1045→-55；残差归因 N82（ORIG 帧
// 0xfc/36 槽 vs 4.4 帧 0x140/55 槽全帧平移，edi/esi/ebx 与 esi/ebx 寄存器分配
// 差）+ N88 装载序 + 少量 4.4 布尔物化（jcc+5）；逐块人工比对语义与 Ghidra C
// 一致（详见 docs/asm_translation_patterns.md 负例表）⇒ N 封顶。
// [b28d-7 N82] ORIG 0x085da882：帧 $0xfc + 槽组 -0x68/-0x6c 起，4.4 帧 $0x140 + 槽组 -0xa8 起（N82 槽复用差异反向放大 ndiff）；P167 CLEAR（153=153）⇒ 语义完成、byte 层封顶。
// [b28d-solo-PvP_Room] 本轮：P225 拆 `P_WIN!=0 && IsPvpObserver!=1` 嵌套 if、P0 去
// `CEventBase* ev` 具名局部（内联 GetRepeatEvent->IsEventing）⇒ 1096→1088；残余 N82
// 帧 $0xfc/36 槽 vs $0x140/55 槽全平移 + 布尔物化。
// [b28d-t3-PoolBigThree] **752→675**（官方 s408 口径，TU 三数不变）：末段座位循环
// `int i=0; while(true){ if (7<i) {channel6 日志; OnEndPvP; break;} …; i=i+1; }`
// 改 `for (int i=0; i<=7; i=i+1) {…}` + 循环后直落代码 —— ORIG 0x085da9a0
// `movl $0,-0x58; jmp COND` + 函数尾 0x085db76b `addl $1; cmpl $7; setle; test; jne
// BODY`（P226 尾测形）；旧 while 形把出口块放循环头并**复制 epilogue**
// （`add $0x140;pop×4;ret` 岛），与 ORIG 尾部单份 fall-through 错位 ~49 行。
// 残余 675 = N82 帧/槽组整体平移 + 布尔物化岛，判词沿用 b28d-7/solo。
void PvP_Room::pvp_reward()   // [b28c-16 SIGFIX] ORIG 串 void
{
    int local_dc[8];
    int local_70 = GetRanking(local_dc);
    int local_6c = GetAce();
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        if (G_GameWorld()->IsFreePvPChannel() != 1)
        {
            CalcuateVictoryPoint(local_dc, local_70);
        }
    }
    unsigned char local_66 = 1;
    unsigned char local_65 = 0;
    for (int local_64 = 0; local_64 < 8; local_64 = local_64 + 1)
    {
        // [b28d-solo-PvP_Room] P225 推广：ORIG 0x085da882 两条件各为独立
        // `test; je <T>` 直跳；`&&` 链在 4.4 恒物化 bool 岛（mov$1/jmp/mov$0/
        // test/je，+5 条）⇒ 拆嵌套 if。
        if (P_WIN(local_64) != 0)
        {
            if (IsPvpObserver(local_64) != 1)
            {
                local_65 = (unsigned char)local_64;
                local_66 = 0;
                break;
            }
        }
    }
    char local_5d = (char)check_acquire_guild_point();
    ((IMatch*)m_matching)->OnReward(this, get_team((int)local_65), local_66 == 1);
    // [b28d-t3-PoolBigThree] P226 尾测形重试（本件 solo 判词未覆盖块布局）：ORIG
    // 0x085da9a0 `movl $0,-0x58; jmp COND` + 尾块 0x085db76b `i++; cmpl $7; setle;
    // test; jne BODY` ⇒ for(i=0; i<=7; i=i+1) 形（旧 while(true)+头部 7<i 判定把
    // 出口块（channel6/OnEndPvP）置于循环头 + 复制 epilogue（`add $0x140;pop×4;ret`）
    // 与 ORIG 函数尾单份 fall-through 块整体错位）。
    for (int local_5c = 0; local_5c <= 7; local_5c = local_5c + 1)
    {
        CUser* local_58 = P_SEATS(local_5c);
        if (local_58 != 0)
        {
            disconnect_detecter::GetInstanceDisconnectDetecter()->UnregisterUser(
                P_SEATS(local_5c));
            local_58->add_pvp_play_info(
                GlobalData::s_systemTime_.getCurSec() - m_battleStartSec, 1);
            bool bVar15;
            if (G_GameWorld()->IsIntegratedPvPBaseChannel() != 0)
            {
                // [b28d-solo-PvP_Room] P0：ORIG 0x085da882 的 GetRepeatEvent 返回值
                // 全程在 %eax（无槽往返，`mov %eax,(%esp)` 直传 this）⇒ 原码是内联
                // 表达式，无 `CEventBase* ev` 具名局部（旧形多 6 条槽中转）。
                if (GlobalData::s_event_manager->GetRepeatEvent(0x38)->IsEventing((CUser*)0) != 0 &&
                    IsPvpObserver(local_5c) == 0)
                {
                    bVar15 = true;
                }
                else
                {
                    bVar15 = false;
                }
            }
            else
            {
                bVar15 = false;
            }
            // ORIG 0x085daa76..（P167 缺 getCurCharacR 同族）：bVar15 由 integrated
            // 频道 eventing 判定独立算出；发包块整体包在 `bVar15 && GetServerGroup()!=0`
            // 条件内（条件臂 + 块内 m_serverGroup + GetServerProxy = 3 次独立调用，P0/P48）。
            if (bVar15 && local_58->GetServerGroup() != 0)
            {
                Packet_Server_Match_data local_a1;
                local_a1.m_serverGroup = (char)(long)local_58->GetServerGroup();
                if (P_WIN(local_5c) == 0)
                {
                    local_a1.m_lose = 1;
                }
                else
                {
                    local_a1.m_win = 1;
                }
                CStatisticServerProxy* proxy =
                    GlobalData::s_statistic_proxy_mgr->GetServerProxy(
                        (ENUM_SERVER_GROUP)(long)local_58->GetServerGroup());
                proxy->SendPacket((char*)&local_a1, 0xd);
            }
            unsigned int local_84 = 0;
            if (((local_5d == 0) || (P_WIN(local_5c) == 0)) ||
                (IsPvpObserver(local_5c) == 1))
            {
                bVar15 = false;
            }
            else
            {
                bVar15 = true;
            }
            if (bVar15)
            {
                local_58->add_guild_point_item();
            }
            CMissionList_Charac* local_48 =
                (CMissionList_Charac*)local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
            local_48->setIssue(P_WIN(local_5c) != 0, m_pvpBattleMode == 2);
            MissionClearCondition_Parameter local_b0((PVP_BATTLE_MODE)m_pvpBattleMode);
            short local_af = (short)local_48->getIssue_mode(m_pvpBattleMode == 2);
            bool local_b1 = false;
            local_48->Update_Win_event(*local_58, local_b0, local_b1);
            if (local_b1 != false)
            {
                const PvpResultType* r = local_58->getPVPResultRefR();
                int local_44 =
                    GlobalData::s_ref_pvp_grade->GetPvpGrade(r->m_expPoint);
                r = local_58->getPVPResultRefR();
                if (r->m_pvpGrade < local_44)
                {
                    PvpResultType* w = local_58->getPVPResultRefW();
                    w->m_pvpGrade = local_44;
                }
            }
            local_58->send_MissionList();
            if (G_GameWorld()->IsPVPChannel() != 0)
            {
                if (G_GameWorld()->IsWinPointPvPChannel() != 0)
                {
                    local_58->add_pvp_result(P_WIN(local_5c) != 0, &local_84);
                    if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
                    {
                        local_84 = 0;
                    }
                    if (P_WIN(local_5c) != 0)
                    {
                        if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
                        {
                            APSystem::CUserProc::ClearActionAndSendtoUser(
                                local_58, (APSystem::_ActionGroupIndex)0x12, 0,
                                (ENUM_PACKETCLASS)0);
                        }
                        else if ((m_pvpBattleMode == 2) || (m_pvpBattleMode == 5))
                        {
                            APSystem::CUserProc::ClearActionAndSendtoUser(
                                local_58, (APSystem::_ActionGroupIndex)0x13, 0,
                                (ENUM_PACKETCLASS)0);
                        }
                        else if (m_pvpBattleMode == 3)
                        {
                            APSystem::CUserProc::ClearActionAndSendtoUser(
                                local_58, (APSystem::_ActionGroupIndex)0x14, 0,
                                (ENUM_PACKETCLASS)0);
                        }
                    }
                }
                else if (G_GameWorld()->GetChannelType() == 6)
                {
                    int local_40;
                    if (P_WIN(local_5c) == 0)
                    {
                        local_40 = -1;
                    }
                    else if (((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3)) ||
                             (m_pvpBattleMode == 5))
                    {
                        local_40 = 1;
                    }
                    else
                    {
                        int local_3c = count_pvp_user();
                        if (local_3c < 4)
                        {
                            local_40 = 1;
                        }
                        else if (3 < local_3c)
                        {
                            local_40 = 2;
                        }
                    }
                    local_58->add_guild_pvp_result(local_40);
                }
            }
            unsigned int local_54 = 0xffff;
            int local_50 = 0;
            int local_88 = 0;
            if (((m_pvpBattleMode == 3) &&
                 (local_50 = m_relayMgr.GetAllKillUserIndex(&local_88), local_50 >= 0)) &&
                (local_50 < 8) && (P_SEATS(local_50) != 0))
            {
                local_54 = (unsigned int)P_SEATS(local_50)->get_unique_id() & 0xffff;
            }
            PacketGuard local_94;
            local_94.clear();
            local_94.put_header(0, 0x2f);
            local_94.put_byte((int)local_65);
            if (G_GameWorld()->GetChannelType() == 0xf)
            {
                void* fair = local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7);
                local_94.put_int(((fair_pvp::CFairPvPScore*)fair)->GetLastTrainingSealCount());
            }
            else if (G_GameWorld()->GetChannelType() == 0x10)
            {
                local_94.put_int(0);
            }
            else if (G_GameWorld()->GetChannelType() == 0xe)
            {
                local_94.put_int(0);
            }
            else
            {
                local_94.put_int(local_58->GetWinPoint());
            }
            local_94.put_byte(local_58->get_pvp_grade());
            if (G_GameWorld()->GetChannelType() == 6)
            {
                local_94.put_int(0);
            }
            else
            {
                int iVar14 = local_58->get_pvp_result_exp_point();
                int local_38 = iVar14 - local_58->get_pvp_result_oldexp_point();
                if ((local_54 != 0xffff) && (P_TEAMS(local_5c) == local_88))
                {
                    int iVar9 = GetTeamCount(local_88);
                    local_38 = local_38 + (iVar9 * 5 * local_38) / 100;
                }
                PvpResultType* w = local_58->getPVPResultRefW();
                // ORIG 0x085dc1ce..（P0/P48 去缓存）：w->m_expPoint 与 LogHistory 的
                // 第 3 参各自独立调用 get_pvp_result_oldexp_point()，不缓存到具名局部
                // （缓存则少 1 call 点，P167 差集 -1 根因）。
                w->m_expPoint = local_58->get_pvp_result_oldexp_point() + local_38;
                local_94.put_int(local_38);
                const PvpResultType* r = local_58->getPVPResultRefR();
                local_58->LogHistory("PvPExp,%d,%d,%d", r->m_expPoint,
                                     local_58->get_pvp_result_oldexp_point(),
                                     local_38);
            }
            int sVar4 = local_58->has_within_Mission();
            if (sVar4 == 0)
            {
                local_58->acceptable_within_mission();
            }
            local_94.put_byte(local_70);
            for (int local_34 = 0; local_34 < local_70; local_34 = local_34 + 1)
            {
                int local_30 = local_dc[local_34];
                if (P_SEATS(local_30) != 0)
                {
                    local_94.put_short((int)P_SEATS(local_30)->get_unique_id() & 0xffff);
                    local_94.put_int(m_field78[local_30]);
                    local_94.put_byte(P_SEATS(local_30)->get_pvp_grade());
                    local_94.put_int(P_SEATS(local_30)->get_pvp_result_exp_point());
                    local_94.put_int(GlobalData::s_ref_pvp_grade->GetPvpCurrRankPoint(
                        P_SEATS(local_30)->get_pvp_grade()));
                    local_94.put_int(GlobalData::s_ref_pvp_grade->GetPvpNextRankPoint(
                        P_SEATS(local_30)->get_pvp_grade()));
                    int local_2c = m_deathMatch.getKillCount(local_30);
                    int local_28 = m_deathMatch.getDeathCount(local_30);
                    local_94.put_int(local_2c);
                    local_94.put_int(local_28);
                    if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
                    {
                        cMyTrace local_80(__PRETTY_FUNCTION__, 0xdab, 0);
                        local_80("<PVP_REWARD> Room(%d), iRank(%d), iRankMember(%d), Player(%s), mode(%d), KillCnt(%d), DeathCnt(%d)",
                                 get_index(), local_34, local_30,
                                 P_SEATS(local_30)->getCurCharacName(), m_pvpBattleMode,
                                 local_2c, local_28);
                    }
                }
            }
            if (local_70 != 0)
            {
                if ((local_6c == -1) || (P_SEATS(local_6c) == 0))
                {
                    local_94.put_short(0);
                }
                else
                {
                    local_94.put_short((int)P_SEATS(local_6c)->get_unique_id() & 0xffff);
                }
            }
            local_94.put_short((int)local_54);
            local_94.put_int((int)local_84);
            local_94.put_byte((int)local_66);
            if (G_GameWorld()->GetChannelType() == 0xf)
            {
                void* fair = local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7);
                local_94.put_short(((fair_pvp::CFairPvPScore*)fair)->GetLastTrainingSealBonusCount());
                fair = local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7);
                local_94.put_short(((fair_pvp::CFairPvPScore*)fair)->GetRemainBonusCount());
            }
            else if (G_GameWorld()->GetChannelType() == 0x10)
            {
                local_94.put_short(0);
                local_94.put_short(0);
            }
            sVar4 = local_58->has_within_Mission();
            local_94.put_byte(sVar4 != 0);
            unsigned char local_49 = 0xff;
            if (m_pvpBattleMode == 3)
            {
                local_49 = (unsigned char)m_relayMgr.GetKillCount(local_5c);
            }
            local_94.put_byte((int)local_49);
            local_94.finalize(true);
            local_58->Send(local_94);
            int mode = get_pvp_battle_mode();
            if (((mode == 4) || (get_pvp_battle_mode() == 5)) && (local_84 != 0))
            {
                int local_b8 = 0;
                int local_bc = 0;
                if (local_58->gain_exp_sp((int)local_84, local_b8, local_bc,
                                          (CUser::eExpAddReason)10, 0, 0) != 0)
                {
                    local_58->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
                }
            }
            long long lVar13 = GlobalData::s_systemTime_.getCurSec();
            local_58->getHackAnalyzer()->setLastMonsterDeadTime(lVar13);
            CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x37);
            if (ev->IsEventing((CUser*)0) != 0)
            {
                CPvPLiveEventData* live = (CPvPLiveEventData*)local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)3);
                live->IncreasePlayCount(local_58);
            }
            const PvpResultType* r2 = local_58->getPVPResultRefR();
            int local_24 =
                GlobalData::s_ref_pvp_grade->GetPvpGrade(r2->m_expPoint);
            if ((local_24 < 1) ||
                (local_58->getPVPResultRefR()->m_pvpGrade == local_24))
            {
                bVar15 = false;
            }
            else
            {
                bVar15 = true;
            }
            if (bVar15)
            {
                PvpResultType* w2 = local_58->getPVPResultRefW();
                w2->m_pvpGrade = local_24;
                CMissionList_Charac* mission = (CMissionList_Charac*)local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
                mission->MakeMissionList_forOldUser(*local_58);
                G_GameWorld()->send_AllBasicInfo(local_58);
            }
        }
    }
    if (G_GameWorld()->GetChannelType() == 6)
    {
        m_guildWarLog.WriteGuildWarPvPLog(m_index, (char)m_pvpBattleMode, m_users);
    }
    ((IMatch*)m_matching)->OnEndPvP(this);
}

// [b28d-7 N95/N82] ORIG 0x085d88ae：栈上对象槽 -0x2c/-0x30，4.4 -0x20/-0x24（槽基差 0xc）+ 帧 0x60 vs 0x50；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] N95-N82: ORIG 0x085d88ae 栈上对象槽 -0x2c/-0x30 vs 4.4 -0x20/-0x24
int PvP_Room::DrawRandomMapIndex(std::vector<int>& out)
{
    bool local_21 = false;
    std::vector<int> local_30;
    std::vector<int>::iterator it = out.begin();
    while (it != out.end())
    {
        if (IsDeathMatchMap(*it, local_21) != 0)
        {
            local_30.push_back(*it);
            it = out.erase(it);
        }
        else
        {
            ++it;
        }
    }
    int local_1c = 0;
    if ((local_30.size() == 0) && (out.size() == 0))
    {
        local_1c = rand() % G_CDataManager()->m_mapList->GetPVPMapCount() + 1;
    }
    else if ((local_30.size() == 0) && (out.size() != 0))
    {
        // [b28d-solo-PvP_Room] P0：ORIG 0x085d88ae 对下标先落具名槽
        // （`mov %eax,-0x14(%ebp); mov -0x14(%ebp),%eax; mov %eax,0x4(%esp)`）
        // ⇒ 原码有具名 int 索引局部，非内联 `out[rand()%size()]`。
        int idx = rand() % out.size();
        local_1c = out[idx];
    }
    else if ((out.size() == 0) && (local_30.size() != 0))
    {
        int idx = rand() % local_30.size();
        local_1c = local_30[idx];
    }
    else
    {
        int idx = rand() % out.size();
        local_1c = out[idx];
    }
    return local_1c;
}

int PvP_Room::SelectRandomMap()
{
    std::vector<int> local_1c;
    GetCandidateMapIndexList(local_1c);
    int local_10 = DrawRandomMapIndex(local_1c);
    ChangePvPMode(local_10);
    return local_10;
}

int PvP_Room::GetAce() const
{
    // ORIG 0x085dc654：m_field98[seat] 为主判据、m_field78[seat] 为平局决胜
    // （mov 0x8(%eax,(seat+0x24)*4) = this+seat*4+0x98；+0x1c → +0x78），
    // 房间内嵌数组，非 CUser 对象字段。
    int local_10 = -1;
    int local_c = -1;
    for (int local_8 = 0; local_8 < 8; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) != 0)
        {
            if (local_c < m_field98[local_8])
            {
                local_c = m_field98[local_8];
                local_10 = local_8;
            }
            else if ((m_field98[local_8] == local_c) &&
                     (m_field78[local_10] < m_field78[local_8]))
            {
                local_10 = local_8;
            }
        }
    }
    return local_10;
}

// [b28d-solo-PvP_Room] AE 归零（90=90，本轮）：ORIG 0x085dc5a3 内层循环只发
// 一条 `movl $0x0,-0x14` ⇒ 原码单处初始化；删 `int local_18 = 0;` 的声明
// 初始化（只留 for 初始化式）后 4.4 槽号集合与 ORIG 全等（旧 N95「槽基差
// 封顶」判词作废——真因是双初始化多 1 条 store 导致后续槽整体浅移 4）。
// 配套：临时 local_14 由函数头移入内层循环体（ORIG -0x10 深槽）。
int PvP_Room::GetRanking(int* out) const
{
    if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
    {
        return m_deathMatch.getRanking(const_cast<CUser**>(m_users), out);
    }
    int local_24 = 0;
    int aiStack_44[8];
    for (int local_20 = 0; local_20 < 8; local_20 = local_20 + 1)
    {
        if (P_SEATS(local_20) != 0)
        {
            // ORIG 0x085dc58e：m_field78[seat]（PvP_Room 内嵌分数数组）
            int local_1c = m_field78[local_20];
            // [b28d-solo-PvP_Room] ORIG 0x085dc5a3 只发一条 `movl $0x0,-0x14`
            // ⇒ 原码单处初始化（for 初始化式），不写声明初始化（双写会多 1 条）。
            int local_18;
            // [b28d-solo-PvP_Room] ORIG 0x085dc5af 内层为独立 break 形
            // （local_10 先入 -0x10 槽，`cmp score; jl EXIT`），尾测 setle。
            for (local_18 = 0; local_18 < local_24; local_18 = local_18 + 1)
            {
                int local_14 = aiStack_44[local_18];
                if (local_14 < local_1c)
                    break;
            }
            for (int local_10 = local_24; local_18 < local_10; local_10 = local_10 - 1)
            {
                out[local_10] = out[local_10 - 1];
                aiStack_44[local_10] = aiStack_44[local_10 - 1];
            }
            out[local_18] = local_20;
            aiStack_44[local_18] = local_1c;
            local_24 = local_24 + 1;
        }
    }
    return local_24;
}

// [b28d-7 N-帧量化] ORIG 0x085dc034 `sub $0x4` vs 4.4 `sub $0x14`：callee PvpResultType::GetTotalScore 定义在 PvpResultType.cpp（跨泳道，仅声明可见）⇒ 4.4 保守 outgoing 量化（P190 附注②）；指令序列全等、P167 CLEAR。
// [b28d-solo-PvP_Room] N-P190: ORIG 0x085dc034 sub $0x4 vs 4.4 $0x14 outgoing 量化；callee PvpResultType::GetTotalScore 跨 TU 仅声明
int PvP_Room::SetPVPResult(int seat, const PvpResultType& result)
{
    // ORIG 0x085dc060：写 PvP_Room 内嵌数组 m_field78[seat] / m_field98[seat]
    // （lea 0x1c(seat),*4 后 mov 0x8(disp) → +0x78；+0x24 → +0x98）。
    // ORIG 085dc040/085dc043：js/jg 各落独立空块岛（nop;jmp end / nop;fall end），
    // 双 goto 保块（同 check_recv_pvp_rank 已验证模式）。
    // 残差仅 outgoing-args reserve 跨版本尺寸差（ORIG sub $0x4 vs 4.4 $0x14），
    // 指令序列全等，N 封顶。
    if (seat < 0)
        goto SET_RES_RANGE;
    if (seat > 7)
        goto SET_RES_RANGE2;
    m_field78[seat] = result.GetTotalScore();
    m_field98[seat] = result.m_avg[0];
SET_RES_RANGE2:
    ;
SET_RES_RANGE:
    ;
}
