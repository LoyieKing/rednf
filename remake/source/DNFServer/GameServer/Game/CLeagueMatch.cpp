// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CLeagueMatch/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CLeagueMatch.h"
#include <algorithm>
#include <string.h>

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "PvP_Room.h"
#include "CGameManager.h"
#include "GameWorld.h"
#include "GlobalData.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "LogManager.h"
#include "GameTypes.h"                  // cMyTrace
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CSchoolServerProxy.h"
#include "SchoolMatchEnums.h"           // ENUM_ELO_RATING_SYSTEM
#include "OnlinePreliminary.h"          // CEloRating / PvPGlobalEnvironmentVariable
#include "Packet_PvPResult.h"           // 结果包 0x80 载荷具名布局（shared/packet/include）
#include "RDARScriptStringManager.h"
#include "TimerDispatcher.h"            // TimerStartMatching
#include "CInventory.h"                 // OS_API::snprintf
// [SweepLeagueMatch4·判据⑪/㉔] ORIG MatchingSystem.cpp 归属：本 TU 定义
// g_schoolMgr / g_guildAgitMgr（static_init 构造点实证；定义由 CGameManager.cpp 迁入）。
#include "CSchoolMgr.h"                 // g_schoolMgr（ORIG B 0x0943e080 / C1 0x0856f206）
#include "CGuildAgitManager.h"          // g_guildAgitMgr（ORIG B 0x0943e0e0 / C1 0x0856f2c6）

// GetGap 排位增量表：ORIG 各 TU 本地 _ZL13g_RankingIncr（16 字节，
// df_game_r nm 显示 15 份内容一致）＝ {1.0f, 1.0f, 1.0f, 1.0f}。
// [SweepLeagueMatch2·P204③] 文件级 static（非匿名 namespace）：后者产
// `_ZN12_GLOBAL__N_1L13g_RankingIncrE`，与 ORIG `_ZL13g_RankingIncr` 不符。
static const float g_RankingIncr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

// [SweepLeagueMatch4·判据⑪ P327 GI 键名 + 判据㉔ 跨 TU 定义迁移]
// ORIG 本 TU（orig_tu_names.tsv:354 标注 MatchingSystem.cpp）的 GI 键名 =
// `_GLOBAL__I__ZN28PvPGlobalEnvironmentVariable25school_match_daily_point_E`
// @0x08568636（size 0x1c）⇒ 该 TU 内**第一个**「强符号·外部链接·本 TU 定义」
// 实体即 school_match_daily_point_。ORIG .data 0x093abb60..0x093abb73 连续
// 五成员，初值读实 1/2/5/3/3（objdump -s -j .data 实证）。
// 迁移前这 5 个定义在 OnlinePreliminaryMatch.cpp，本 TU GI 键名误为
// `_GLOBAL__I__ZN12CLeagueMatch17pWaitingRoomList_E`（ORIG 无 ⇒ 计入 missing）。
// 顺序铁律：本组必须置于 pWaitingRoomList_ **之前**（P327：仅定义行序决定键名）。
int PvPGlobalEnvironmentVariable::school_match_daily_point_ = 1;
int PvPGlobalEnvironmentVariable::school_match_daily_point_weekends_ = 2;
int PvPGlobalEnvironmentVariable::school_match_fight_count_for_gain_point_ = 5;
int PvPGlobalEnvironmentVariable::school_match_elo_k_ = 3;
int PvPGlobalEnvironmentVariable::school_match_elo_k2_ = 3;

// [SweepLeagueMatch4] ORIG static_init @0x085685a0（size 0x96）构造序实证：
//   `_ZStL8__ioinit`(0x943e172, call 0x85685c0) →
//   `g_schoolMgr`(0x943e080, call 0x85685e2 = CSchoolMgr::C1 0x0856f206) →
//   `g_guildAgitMgr`(0x943e0e0, call 0x8568612 = CGuildAgitManager::C1 0x0856f2c6)，
// 各带 __cxa_atexit。两对象定义由 CGameManager.cpp 迁回本 TU（声明序 = 构造序）。
CSchoolMgr g_schoolMgr;
CGuildAgitManager g_guildAgitMgr;

CLeagueMatchRoomList* CLeagueMatch::pWaitingRoomList_ = 0;   // B 0x0943e064
int CLeagueMatch::nWaitingRoomListRefCount_ = 0;             // B 0x0943e068
int CLeagueMatch::nextAuthValue_ = 0;                        // B 0x0943e06c
bool CLeagueMatch::bMatchable_ = false;                      // B 0x0943e060
unsigned char CLeagueMatch::nBattleMode_ = 3;                // D 0x093abb75 初值 3
unsigned char CLeagueMatch::nPlayerCountPerOneTeam_ = 2;     // D 0x093abb74 初值 2

// ---------------------------------------------------------------------------
// PlayerInfo / TeamInfo（报告 TeamInfo/*.md + ORIG objdump 转写）
// ---------------------------------------------------------------------------

// [b28c-21] P210 定义顺序：ORIG TeamInfo::reset `sub $0x28` 保守帧；4.4 对
// 已见定义的 PlayerInfo::reset 精确继承 $0x14 ⇒ 本定义前置于 callee 之前。
// [A11-round5 复核] N125 证据加强（本 TU 12 件的同一残差族）：本轮以 8 源形探针
// （`&arr[i]` 索引 / `(char*)t + i*0x1c` 指针算术 / 具名 int 中间量 / 二维下标 /
// unsigned 索引 / 数组退化 + 标量偏移）在 4.4.6 逐形复编（/tmp/r5/probeD.cpp f1..f8），
// 恒产 `shl $0x2,%eax; lea 0x0(,%eax,8),%edx; sub %eax,%edx`，**无一**产 ORIG 的
// `lea 0x0(,%eax,4),%edx; lea 0x0(,%edx,8),%eax; sub %edx,%eax`（TeamInfo::size
// 0x0855cd42、GetUser 0x0855e5f4、GetGap 0x0855e1f9、CheckMatchable 0x0855fa2a、
// OnLeaveRoom 0x0855d2xx、_RegisterWaitingRoom 0x0855d94b 等族）⇒ N125 结构性豁免。
// 追加 4 形（probeD2.cpp f9..f13：成员数组元素取址直传 out-of-line callee / `arr+i`
// 指针加法 / `arr[i].field` 字段访问 / `reinterpret_cast<Ent*>((char*)t+i*0x1c)` 偏移链）
// 仍恒 `shl $0x2` ⇒ 12 源形全败，后继勿再试。
// [A11-round5 C05/D08 复查 · 无落点] ①范式①（尾 fail 块）：本 TU 18 件 DIFF 的 ORIG
// 尾 16 条独立 `mov $0/$1,%eax` 均为正常返回值块（GetUser/GetSchoolNo/OnMatchRoom/
// CheckMatchable/TeamInfo::add·del/_getSameSchool 等），我方同形；②范式③（早返中继岛）
// 与 D08（多守卫同目标合并）：全 33 件 diff 的 `jmp` 行差逐件为 0 ⇒ 无岛可消；
// ③范式②（F2）：「ORIG `cmp mem,reg`」行（TeamInfo 族 stride 链）两侧同为内存直比，
// 残差仅槽号/lea-vs-shl，非装载序 ⇒ 不适用。
// [b28d-solo-CLeagueMatch] DIFF 19/20·N-STRIDE（S180 定稿）：stride 0x1c 的 i*4
// ORIG 用 lea(,%eax,4)、4.4 恒 shl $2（探针 /tmp/b28d_cm/probe_stride.cpp 11 形 +
// probe2.cpp 7 索引类型 + 4.4.6 后端 + 8 旗标全败）⇒ 4.4 多 1 条 mov %edx,%eax
// 中转；语义等价，结构性豁免。
// [J9MgrTimerCLeagueMatch.V1 2026-09-11 终局 · 全族统一证据/勿再烧轮次]
// 本 TU 非 identical 16 件的残差已全量归因，两条结构性来源：
//  (a) N-STRIDE（12 件）：ORIG `lea 0x0(,%eax,4),%edx`（×4 用 scale=4 的 lea）vs
//      4.4 恒 `shl $0x2,%eax`（+1 条中转 mov）。
//  (b) synth_mult copy 链（3 件：OnCompleteLoad/_SendMatchStatePacket/OnReward）：
//      ORIG `mov %ecx,%eax; mov %eax,%ecx`（纯冗余，删去逐条等价）。
//  本轮新增实证（全部失败）：`#pragma GCC target` 8 形（tune=i386/i486/pentium/
//  pentiumpro · arch=i386/i486/pentiumpro · tune=generic）、`#pragma GCC optimize`
//  3 形（strength-reduce / O1 / web）、cc1plus **4.4.6 后端** A/B（头集 4.4.6 +
//  -march=i486，独立 /tmp/tu_CLeagueMatch_446.o，逐件读数与本 TU 全同）、以及
//  70 源形探针（/tmp/probe_ocl{1,2,3}：数组/指针/char* 偏移链/sizeof/两层引用/
//  具名中间量/register/unsigned·cast 索引/上界写法/加法序）⇒ 4.4.x 全系（含 O1）
//  不产该两类形；ORIG 侧应为 4.1 系后端产物。
//  **已试尽**：源码形/函数级 pragma/后端版本三维。**未试**：换 cc1plus 到 4.1/3.4
//  变体（本 TU 当前路由 `$C6`+i486，属 dnf_game_cxx.sh 路由/Main 决策，禁改）。
//  语义轴：16 件 p167 全绿（12 CLEAR + 4 件仅 C1/C2·D1/D2 同址别名），另修复
//  OnCreateRoom 韩文日志串实字节（UTF-8→cp949，§6.5，ORIG 文件偏移 0xb7a2c8/0xc5a23c）。
#include "CLeagueMatch/reset.inc"
{
    for (int i = 0; i <= 3; ++i)
        m_players[i].reset();
}

// [SweepCLeagueMatch-2·判据㉓] PlayerInfo::reset（ORIG W 0x085687d4）已改类内
// inline 定义（CLeagueMatch.h），由使用点发射 W；本 TU 因 TeamInfo 族调用仍发射
// 该 W 副本。函数体逐条不变。

// [b28d-lane6] AE（已修）：m_flag bool 化（P248 档②）+ `==` 正向包裹（P222）后
// 第一/二段与 ORIG 0x0855c95a 全等；x87 比较段经 fresh 重编译后亦归一（34=34）。
#include "CLeagueMatch/CompareRating.inc"
{
    // b28d-lane6：m_flag bool 化（P248 档②）复现第一段 ORIG 0x0855c95a 双重 movzbl
    // + cmpl/jle；第二段改 `==` 正向包裹复现 `jne` 落 false 尾块（P222/P47）；
    // rating 段用 if/return true 保留 ORIG 的 seta/test/je 物化（P56）。
    if (a.m_flag > b.m_flag) return true;
    if (a.m_flag == b.m_flag)
    {
        if (a.m_rating > b.m_rating) return true;
    }
    return false;
}

// [b28d-solo-CLeagueMatch] DIFF 100/100·N-STRIDE：同 TeamInfo 族 stride 差异
// （ORIG lea 链 vs 4.4 shl 链，逐访问点 4 条对 4 条），语义等价，结构性豁免。
// [J9MgrTimerCLeagueMatch.V1 2026-09-11 终局] 本轮两旋钮：①具名 `CUser* u` 缓存
// m_players[i].m_user（P0 反向）⇒ our 100→81、ORIG-only/OURS-only 43→64 劣（ORIG
// 每访问点重算 m_user，无缓存槽），已回滚；②循环上界 `i <= 3` → `i < 4` ⇒ 4.4 折叠
// 同形（43/43 不变），已回滚。ORIG 0x0855c9c2..0x0855caf7 的 9 处 `lea 4/lea 8/sub`
// 链 = N125 结构性（证据见 OnCompleteLoad 探针网络）【不可翻】。p167 4/4 CLEAR。
#include "CLeagueMatch/refresh.inc"
{
    for (int i = 0; i <= 3; ++i) {
        if (m_players[i].m_flag != 0) {
            m_players[i].m_accId = m_players[i].m_user->get_acc_id();
            m_players[i].m_rating = m_players[i].m_user->GetRating();
            m_players[i].m_schoolNo = m_players[i].m_user->GetSchoolNo();
            m_players[i].m_user->GetDailyBadge(m_players[i].m_badges);
        }
    }
}

// [b28d-solo-CLeagueMatch] DIFF 91/91·N-STRIDE：xor $1 守卫形已按 P292/P248 修复
// （`if (!m_flag)` 正向包裹），残差全为 stride 0x1c 的 shl-vs-lea，语义等价。
#include "CLeagueMatch/add.inc"
{
    // [G9WorldTimerLeague 2026-09-10 复核] TeamInfo 族（add/del/refresh/reset/size，
    // 统一 stride 0x1c）成员 disp 扫描：唯一 OURS-only {0x10:1} 为 m_user 成员
    // 偏移 `lea 0x10(%edx),%eax` 的两步形，ORIG 0x0855cb6e 段以
    // `lea 0x0(,%eax,4),%edx + lea 0x0(,%edx,8),%eax + sub %edx,%eax` 一步到位
    // ⇒ N125（i*4 恒 shl $2，7 索引类型 + 双后端实测全败）+ N4/N107 族，
    // 无语义缺口；p167 CLEAR（4/4）。
    // [b28d-solo-CLeagueMatch] P292/P248 档②：ORIG 0x0855cb83 `movzbl; xor $1; test; je`
    // = bool 字段 + 裸 `if (!m_flag)` 正向包裹（P296）；continue 形反产 xor+jne 岛。
    for (int i = 0; i <= 3; ++i) {
        if (!m_players[i].m_flag) {
            m_players[i].m_flag = 1;
            m_players[i].m_user = user;
            m_players[i].m_accId = user->get_acc_id();
            m_players[i].m_rating = user->GetRating();
            m_players[i].m_schoolNo = user->GetSchoolNo();
            user->GetDailyBadge(m_players[i].m_badges);
            return 1;
        }
    }
    return 0;
}

// [b28d-solo-CLeagueMatch] DIFF 39/39·N-STRIDE：残差全为 stride 0x1c 的
// shl-vs-lea（3 访问点），语义等价，结构性豁免。
#include "CLeagueMatch/del.inc"
{
    for (int i = 0; i <= 3; ++i) {
        if (m_players[i].m_flag != 0 && m_players[i].m_user == user) {
            m_players[i].m_flag = 0;
            return 1;
        }
    }
    return 0;
}

// [b28d-solo-CLeagueMatch] DIFF 23/23·N-STRIDE：stride 0x1c 的 i*4 ORIG 用
// lea(,%eax,4)、4.4 恒 shl $2（S180 探针 11 形 + 7 索引类型 + 4.4.6 + 8 旗标
// 全败），语义等价。
#include "CLeagueMatch/size.inc"
{
    /* ORIG @0855cd42 乘法链 lea(,%eax,4)/lea(,%edx,8)/sub %edx（PlayerInfo
     * stride 0x1c）；4.4 恒把 i*4 折成 shl $2（probe：显式 unsigned long
     * 两步 stride 同产 shl），指令选择成本模型差，N 类不可翻，保持具名
     * 字段下标访问【§4.3】 */
    int n = 0;
    for (int i = 0; i <= 3; ++i)
        if (m_players[i].m_flag != 0)
            ++n;
    return n;
}

// ---------------------------------------------------------------------------
// 生命周期
// ---------------------------------------------------------------------------

// ORIG C1/C2 0x0855cd74：IMatch ctor → PVP_ROOM_INFO@+8 → 懒建 pWaitingRoomList_
// （new 0x1c 置零后 C1 0x08568fb0）→ 引用计数自增。
// [b22判词·N族] ORIG new(0x1c) 后、C1 call 前有 6 条 value-init 标量置零
// （+0x00/08/0c/10/14/18），GCC4.1 语义；4.4 视 user-provided ctor 类免置零直调。
// placement-new+try 实测反引入 operator new(void*,ptr) call + EH spill，C++98
// 无法复现该置零序列，语义等价（残差 6 insns），按 N 族结案，勿再开轮。
// [b28d-solo-CLeagueMatch] AE（TU 口径）：67=67 全等。原 N77 残差（new
// PVP_ROOM_INFO 的 6 条 value-init 清零 +0x00/08/0c/10/14/18）随 PvPTypes.h
// PVP_ROOM_INFO 保持「无用户 ctor」而恢复（P299：用户 ctor 抑制 value-init）；
// EH 尾块序亦已归一。本件无需再改。
#include "CLeagueMatch/CLeagueMatch.inc"
{
    if (pWaitingRoomList_ == 0)
        pWaitingRoomList_ = new CLeagueMatchRoomList();
    ++nWaitingRoomListRefCount_;
}

// ORIG D1 0x0855ce4c：引用计数递减，归零且单例仍在时 operator delete（不析构、
// 不置空）；随后 PVP_ROOM_INFO / IMatch 析构由编译器生成。
#include "CLeagueMatch/dtor_CLeagueMatch.inc"
{
    --nWaitingRoomListRefCount_;
    if (pWaitingRoomList_ != 0 && nWaitingRoomListRefCount_ == 0)
        ::operator delete(pWaitingRoomList_);
}

// ---------------------------------------------------------------------------
// IMatch 覆盖
// ---------------------------------------------------------------------------

// [b28d-solo-CLeagueMatch] DIFF 72/72·N-STRIDE：三守卫已按 P296 改嵌套正向 if，
// 与 ORIG 0x0855e1a3/a9/af 同落 +0xcb（消 3 个 jmp+nop 岛）；残差全为 stride 的
// shl-vs-lea，语义等价。
#include "CLeagueMatch/GetGap.inc"
{
    float gap = 0.0f;
    for (unsigned int i = 0; i < (unsigned int)nPlayerCountPerOneTeam_; ++i) {
        CUser* mine = m_teamA.m_players[i].m_user;
        CUser* theirs = other->GetUser(i);
        // [b28d-solo-CLeagueMatch] P296：ORIG 三守卫各 `je` 直跳循环尾（无 continue
        // 的 jmp+nop 中继岛，0x0855e1a3/a9/af 同落 +0xcb）⇒ 嵌套正向 if 复现。
        if (m_teamA.m_players[i].m_flag != 0) {
            if (mine != 0) {
                if (theirs != 0) {
                    float a = mine->GetRating() * g_RankingIncr[i];
                    float b = theirs->GetRating() * g_RankingIncr[i];
                    gap += __builtin_fabsf(a - b);
                }
            }
        }
    }
    return gap;
}

// [b28d-solo-CLeagueMatch] DIFF 29/29·N-STRIDE：bool 化 + 裸 `if(!m_flag)` 的
// xor 段已与 ORIG 0x0855e5e9 一致；残差为 stride 0x1c 的 shl-vs-lea，语义等价。
#include "CLeagueMatch/GetUser.inc"
{
    if (idx > 3)
        return 0;
    // b28d-lane6：m_flag bool 化 + 裸 `if(!field)` ⇒ ORIG 0x0855e5e9 的
    // `movzbl 0x18; xor $1; test %al,%al; je`（P248 档②，探针 /tmp/l6p/p6.cpp f2）。
    if (!m_teamA.m_players[idx].m_flag)
        return 0;
    return m_teamA.m_players[idx].m_user;
}

#include "CLeagueMatch/GetUserList.inc"
{
    *reinterpret_cast<TeamInfo*>(out) = m_teamA;
}

#include "CLeagueMatch/RequestMap.inc"
{
    mapNo = 3;
    if ((int)mode != 6)
        mode = static_cast<PVP_BATTLE_MODE>(nBattleMode_);
}

#include "CLeagueMatch/GetExtraWinPoint.inc"
{
    // 活动期窗口（0x4911b4f0, 0x492429ef]，期内返回额外分否则 0。
    // ORIG 两次调用 getCurSec（P48：O0 无 CSE，条件双臂各调一次）。
    if (GlobalData::s_systemTime_.getCurSec() > static_cast<int>(0x4911b4f0) &&
        GlobalData::s_systemTime_.getCurSec() <= static_cast<int>(0x492429ef))
        return extra;
    return 0;
}

#include "CLeagueMatch/OnCreateRoom.inc"
{
    nextAuthValue_ += 1;
    m_authValue = nextAuthValue_;
    m_state = 0;
    m_teamB.reset();
    m_teamA.reset();
    m_teamA.add(user);
    return 0;
}

// 【N7 封顶】b26 lane19：虚调用 callee 寄存器 %ecx↔%eax 互换（ORIG
// mov (%eax),%ecx+call *%ecx vs 4.4 mov (%eax),%eax+call *%eax，16 条逐条
// 长度/助记符全等），临时/引用/限定名不可翻，语义同。
#include "CLeagueMatch/OnDestroyRoom.inc"
{
    m_state = 0;
    pWaitingRoomList_->DelRoom(this);               // IRoomList vtable+0x04
}

#include "CLeagueMatch/OnJoinRoom.inc"
{
    if (m_state != 3)
        m_teamA.add(user);
}

// ORIG T 0x0855d1c0
// [b28d-solo-CLeagueMatch] DIFF 142/142·真 bug 已修 + N-STRIDE：ORIG 0x0855d245/
// d273 以**第 3 参** team 判 m_teamA/m_teamB（原码误用第 4 参 flag，调用点
// OnWalkOut/PvP_Room 均传 (user,room,team,seat)）；本波再修两处 P0：
// ①`_ReturnTeam(room, room->get_manager_team(), user)` 内联实参（ORIG
// 0x0855d1fc 返回值 %eax 直压 0x8(%esp)，无 managerTeam 具名槽）；
// ②`int idx = 0;` + `for (idx = 0;…)` 双初始化（ORIG 0x0855d232/d239 两次
// movl $0,-0xc）且删 selfRating/oppoRating 的 0.0f 初始化（ORIG 无该清零，
// P171）；gap -5→0，残差 6 处 stride shl-vs-lea（N-STRIDE；p167 7/7 CLEAR），
// 语义等价。
#include "CLeagueMatch/OnLeaveRoom.inc"
{
    if (m_state == 4) {
        _SendMatchStatePacket(4, 2);
        m_state = 0;
        // [b28d-solo-CLeagueMatch] P0：ORIG 0x0855d1fc..0x0855d219 的
        // get_manager_team() 返回值在 %eax 直压 0x8(%esp)（无具名局部槽往返）
        // ⇒ 内联实参，删 managerTeam 局部。
        _ReturnTeam(room, room->get_manager_team(), user);
    } else if (m_state == 5) {
        // [b28d-solo-CLeagueMatch] P191：ORIG 0x0855d232/d239 两次 `movl $0x0,-0xc`
        // = 声明初始化 + for 初始化双形；P171：ORIG 无 selfRating/oppoRating 的
        // float 0.0f 清零（仅 idx 清零），未命中时由 idx<=3 保护 ⇒ 删初始化式。
        int idx = 0;
        float selfRating;
        float oppoRating;
        for (idx = 0; idx < 4; ++idx) {
            // [b28d-solo-CLeagueMatch] 真 bug 修正：ORIG 0x0855d245/d273 以**第 3 参**
            // `cmpl $0x1,0x14(%ebp)` 判队（m_teamA +0x24 / m_teamB +0x94），原码误用
            // 第 4 参 flag（0x18）——调用点 OnWalkOut/PvP_Room 均传 (user,room,team,seat)。
            if (team == 1) {
                if (m_teamA.m_players[idx].m_user == user) {
                    selfRating = m_teamA.m_players[idx].m_rating;
                    oppoRating = m_teamB.m_players[idx].m_rating;
                    break;
                }
            } else if (m_teamB.m_players[idx].m_user == user) {
                selfRating = m_teamB.m_players[idx].m_rating;
                oppoRating = m_teamA.m_players[idx].m_rating;
                break;
            }
        }
        if (idx <= 3) {
            CEloRating::calc(selfRating, oppoRating,
                             static_cast<ENUM_ELO_RATING_SYSTEM>(0),
                             PvPGlobalEnvironmentVariable::school_match_elo_k_);
            user->UpdateRating(selfRating);
        }
    } else {
        m_state = 0;
    }

    if (team == 1)
        m_teamA.del(user);
    else
        m_teamB.del(user);

    pWaitingRoomList_->DelRoom(this);               // IRoomList vtable+0x04
}

// ORIG T 0x0855db58。ORIG 返回 int：m_state==1 → 1；失败路径 → 0；成功 → 1。
#include "CLeagueMatch/OnRegisterMatch.inc"
{
    if (m_state == 1)
        return 1;

    if (!bMatchable_) {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(199), 0x13);
        return 0;
    }

    // ORIG movl $0x0,-0xc 预清（P198③ 声明=0+独立赋值两句式）。
    int err = 0;
    err = _CheckMatchable(room);
    // P225：ORIG `cmpl $0,slot; setne %al; test %al; je <主流程>` bool 四段物化
    // → while+return 形精确复现（if 直判被 4.4 折成 cmp;je 两段）。
    while (err != 0) {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(199),
                                 static_cast<unsigned char>(err & 0xff));
        return 0;
    }

    if (m_state != 2)
        _RegisterWaitingRoom(room);

    m_state = 1;
    // N96 清绑：ORIG mov 0x18(%eax),%ebx 跨 get_index call 缓存 m_authValue（P87）；
    // 成员表达式直传（P0 无具名局部）。
    TimerStartMatching::registNextTimer(10, room->get_index(), m_authValue);
    _SendMatchStatePacket(0, 1);
    return 1;
}

// ORIG T 0x0855dc64。ORIG 返回 int：失败路径 → 0；成功 → 1。
// [b28d-solo-CLeagueMatch] DIFF 277/277·ndiff 102·真语义 bug 已修 + N95
// （S217 新鲜 .o 口径；p167 29/29 CLEAR）：①0x2c2 守卫
// 原用 theirRoom->get_player_count()，ORIG 0x0855de5e `mov 0xc(%ebp),%eax` +
// Ghidra C `get_player_count(param_1)` 实证为**形参 room** ⇒ 改回 room（真 bug）；
// ②leader/theirRoom 按 ORIG 0x0855dca0/dca7 函数头预清 0（P198③，gap 2→0）；
// ③五处 cMyTrace 改全表达式临时链（P221）翻正槽方向（ORIG -0x6c→-0x2c 由深到浅，
// 原具名形 -0x38→-0x78 反向）；残差为全帧槽基差 8（N95：4.4 槽基恒 ebp-8 起、
// 4.1 可顶到 ebp）与 setg/test 物化差；p167 CLEAR（29/29），语义闭合。
// [b28d-solo-CLeagueMatch·S408] 复验 ndiff 102 不变。本轮实测**回滚**一项：
// ORIG 0x0855e009/e010 双预清（theirTeam=-0x14 / seat=-0x10）的 P191 双声明形
// （`int theirTeam=0; unsigned int seat=0; for(seat=0;…)`）⇒ ndiff 102→121
// （+2 insn，槽次数反劣）⇒ 回滚保持 for 内声明。剩余槽差 2 枚：badSize 具名槽
// （ORIG 无槽，纯寄存器物化，S217 试 P226 语句表达式亦反劣）+ leader/member 槽
// 复用（ORIG 共用 -0x1c(9)，4.4 分开 -0x24(4)/-0x18(3)）⇒ N95+N82 封顶。
// [J9MgrTimerCLeagueMatch.V1 2026-09-11 终局] 本轮再补两旋钮（均**中性**⇒回滚）：
// ① 循环内三处 `static_cast<int>(seat)` 去除（改为 unsigned→int 隐式转换）⇒
//    278/278、ORIG-only/OURS-only=177/177 逐项不变（4.4 转换 pseudo 等价）；
// ② `int theirTeam` → `unsigned int`（P290 局部 unsigned 升格）⇒ 同上完全不变。
// 残差构成经 -dr 归因复核：N125（lea-vs-shl ×10）+ N95 槽基平移 + trace 立即数拆分
// （P227 全表达式链已用）+ setg/test 物化，无语义缺口【可翻项已试尽】。
#include "CLeagueMatch/OnMatchRoom.inc"
{
    if (m_authValue != authValue) {
        // [b28d-solo-CLeagueMatch] P221：ORIG 五处 trace 槽 -0x6c/-0x5c/-0x4c/
        // -0x3c/-0x2c（由深到浅），具名声明形 4.4 反向（-0x38→-0x78）⇒ 改
        // 「构造即调用」全表达式临时链翻槽方向。
        cMyTrace(__PRETTY_FUNCTION__, 0x295, 0)("OnMatchRoom timeout");
        return 0;
    }
    if (m_state != 1)
        return 0;

    // [b28d-solo-CLeagueMatch] P198③：ORIG 0x0855dca0/dca7/dcb4 三处预清
    // （Ghidra C：leader=0; theirRoom=0; opponent=0 在 Match 调用前）
    // ⇒ 函数头声明并置 0（双槽置 0 形），后文赋值；补 gap 2。
    CUser* leader = 0;
    PvP_Room* theirRoom = 0;
    IMatch* opponent = 0;
    pWaitingRoomList_->Match(this, &opponent);   // ORIG 丢弃返回值，仅判 opponent==0
    if (opponent == 0) {
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    leader = opponent->GetUser(0);
    if (leader == 0) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2b0, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2b0);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    theirRoom = reinterpret_cast<PvP_Room*>(leader->GetPVPRoom());
    if (theirRoom == 0) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2b9, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2b9);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    // [b28d-solo-CLeagueMatch] 真语义 bug 修正（Ghidra C `get_player_count(param_1)`
    // + ORIG 0x0855de5e `mov 0xc(%ebp),%eax` 实证）：此守卫检查的是**形参 room**
    // 的人数，非 theirRoom（theirRoom 仅用于下方 badSize）。
    if (room->get_player_count() > (int)nPlayerCountPerOneTeam_) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2c2, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2c2);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    bool badSize =
        (m_teamA.size() > (int)nPlayerCountPerOneTeam_) ||
        (theirRoom->get_player_count() > (int)nPlayerCountPerOneTeam_);
    // [SweepLeagueMatch4·L11 round1 补充实测] badSize 直接内联进 if（去具名 bool）
    // ⇒ ndiff 79→**111**、insn 277→276（劣化，已回滚）：ORIG 该处虽无槽，但 4.4 的
    // 具名 bool 槽恰与 ORIG 的匿名物化区重合 ⇒ 保留具名形为最优可接受形（N82/P332）。
    if (badSize) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2ca, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2ca);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    _BackupRoomInfo(theirRoom);
    m_teamB.reset();
    theirRoom->get_user_list(&m_teamB);
    _SendMatchStatePacket(3, 2);
    m_state = 3;
    // [b28d-solo-CLeagueMatch·S408 实测] ORIG 0x0855e009/e010 双预清（-0x14/-0x10）
    // 的 P191 双声明形（theirTeam/seat 前置 + for 再初始化）本轮实测 ndiff
    // 102→121（+2 insn，槽次数反劣）⇒ **回滚**，保持 for 内声明形。
    for (unsigned int seat = 0; seat <= 7; ++seat) {     // setbe 无符号 <=7
        CUser* member = reinterpret_cast<CUser*>(theirRoom->get_user_seat(static_cast<int>(seat)));
        if (member == 0)
            continue;
        int theirTeam = theirRoom->get_team(static_cast<int>(seat));
        _PullOutRoom(member, theirRoom, static_cast<int>(seat));
        int newSeat = _PutInRoom(member, room, false);
        if (theirTeam == 3)
            room->OnSetPVPSeatState(member, newSeat,
                                    static_cast<ENUM_SEAT_STATE>(3),
                                    static_cast<ENUM_WALKOUT_TYPE>(2));
    }
    pWaitingRoomList_->DelRoom(this);                   // IRoomList vtable+0x04
    m_state = 4;
    _SendMatchStatePacket(2, 2);
    return 1;
}

#include "CLeagueMatch/OnStartFight.inc"
{
    m_state = 5;
}

// ORIG T 0x0855e55a
// [b28d-solo-CLeagueMatch] DIFF 45/45·ndiff 2·N-P75（S217 新鲜 .o 复验）：
// ORIG 0x0855e594 第二实参先入 %eax 再压常量 0，4.4 恒先压常量；cast/NULL/
// 具名局部 3 形不可翻（S180 追加逗号求值序形 ((void)managerTeam,(CUser*)0)
// 亦不变）。S217 再补 2 形：`0L` 与 `0` 同形；`*&managerTeam` 虽消实参序差但
// 把 `cmpl $1,-0x1c(%ebp)` 变 `mov;cmp $1,%eax`（46 条 +1，净零）⇒ 回滚。
// 共 6 形全败，语义等价。
#include "CLeagueMatch/OnEndFight.inc"
{
    unsigned int managerTeam = room->get_manager_team();
    m_state = 0;
    // b28d-lane6 [N-P75]：ORIG 0x0855e594 先 `mov -0x1c(%ebp),%eax`（第二实参装载）
    // 再 `movl $0x0,0xc(%esp)`（第三实参常量压栈）；旧判词「cast/NULL/具名局部
    // 3 形不可翻」**作废**。
    // [FL8PvPWorldLeague 2026-09-10] P290 生效：局部 `int`→`unsigned int` 使该实参
    // 升格 callee-saved/提前物化，复现 ORIG 的「先载第二实参、后压常量」序。
    // 本件 DIFF→AE：TU 四数 identical=44 ae=61→**62** near=1 diff=18→**17**，
    // 单函数 AE-EQ（glm_one 0 残差行）。
    _ReturnTeam(room, managerTeam, 0);
    if (managerTeam != 1)
        m_teamA = m_teamB;   // ORIG rep movsl 0x1c dwords（P83 结构整体赋值）
    m_teamA.refresh();
    m_teamB.reset();
}

// ORIG T 0x0855e4c4。ORIG 返回 1。
// 【N7 封顶】b26 lane19：虚调用 callee 寄存器 %ecx↔%eax 互换 + 实参装载序
// 跨版本差（ORIG 先槽 -0x10 后形参 0x10，4.4 反序——N80 族不可翻），33 条
// 长度/助记符全等。
// [b28d-solo-CLeagueMatch] NEAR 33/33·N7（S180 复验）：虚调用目标
// %ecx(ORIG 0x0855e4dc)↔%edx(4.4.7) + 实参装载序（P75）；33 条助记符逐条全等，
// 语义等价。
// [J9MgrTimerCLeagueMatch.V1 2026-09-11 终局] 本件唯一可翻路径 = fn 落 %ecx（ORIG
// 0x0855e4fc `mov (%eax),%ecx`）。本轮两旋钮：①`static_cast<IMatch*>(this)->OnLeaveRoom(...)`
// 使 4.4 产 `mov (%eax),%ecx`（fn 对齐 ✓）但接收者提前载入 %eax ⇒ 实参全改 %edx，
// 行差 6/6→9/9 **净劣**，已回滚；②cc1plus **4.4.6 后端** A/B（头集 4.4.6 + i486，
// 独立 /tmp/tu_CLeagueMatch_446.o）逐件读数与本 TU 全同（44/63/1/16）⇒ 后端维度
// 无解。根因 = i386 REG_ALLOC_ORDER 差（ORIG 首选 %ecx、4.4.7 首选 %edx），源码层
// 不可控【已试尽】。p167 2/2 CLEAR。
#include "CLeagueMatch/OnWalkOut.inc"
{
    int team = room->get_team(seat);
    CUser* user = reinterpret_cast<CUser*>(room->get_user_seat(seat));
    OnLeaveRoom(user, room, team, seat);                // vtable+0x2c 四参形态
    return 1;
}

// ORIG T 0x0855e52a。ORIG 返回 bool（!ready && state==4 → 0）。
#include "CLeagueMatch/OnReady.inc"
{
    if (!ready && m_state == 4)
        return false;
    return true;
}

// ORIG T 0x0855eb22（结算奖励全流程）
// §6.2 巨件核销：P167 45/45 [0缺0多]（修前 42/45）；gap 79→35；残差归因 N86/N82
// （t*0x70/j*3 寻址代数分解跨版本差：ORIG shl$4+lea+sub 折乘链 vs 4.4
//   shl$3+add，纯分解形态、语义同；全帧槽号平移）。
// 真修三处：
//   1) 结果包改栈对象 Packet_PvPResult res 走真 ctor（W 0x08568dc6≡
//      PacketHeader(0xfa5,0x80)，重建侧符号已在）；原手写展开的
//      reversed1/m_connNo 清零属发明语义（ORIG ctor 不清），删。
//   2) 实时徽章回填条件修正：ORIG 0x0855f539 固定寻址 this+i*0x1c+0x98/0x94
//      = teamB 的 m_flag/m_user（与 t 无关），原实现错挂 t 分支。
//   3) GetDailyBadge 由循环改 3 条独立调用（ORIG $0x0/$0x1/$0x2 立即数直传
//      三处 call，P0），读取 user 按数组化寻址 (&m_teamA)[t].m_players[i].m_user
//      复现 ORIG 乘法寻址链（无具名缓存槽）。
// [b28d-solo-CLeagueMatch] DIFF 992/960·ndiff 1048·N82（S217 新鲜 .o 口径）：
// 帧 0x1fc vs 0x24c，
// 4.4 槽复用策略差（§6.2 三件套口径：p167 调用点差集 45/45 CLEAR [0 缺 0 多]、
// gap 34 归因槽复用与 t*0x70/j*3 寻址代数分解；S180 试 P235 union 视角
// m_teams[t] 该件反劣（1390→1398）⇒ 两形取优保留 (&m_teamA)[t]；剩余全为
// 槽号平移与 shl-vs-lea；语义完成、byte 层 N 封顶）。
// [b28d-solo-CLeagueMatch·S408 定稿] **1048→567**（−481）：帧 0x1fc/0x1fc 全等、
// 22 槽集合逐槽全等、指令数 992/990、p167 45/45 CLEAR。修法=六处 P0/P183
// 内联（eloK2/winType/playCount/TeamInfo&/PlayerInfo&/schools/mySchool/fmt/st/
// winFlagLocal）+ P191 t/i 双初始化复用槽 + `int type;` 去初始化式 + 绿章三守卫
// 负向 continue（P224/P296）+ **真 bug** 删 UpdateRating 前多余的 m_user!=0 守卫
// （P140/P171，ORIG 0x0855f7d3..f819 只查 m_flag）+ ratings 等宽 union int 视角
// 复现 ORIG 单段零初始化。剩余 567 = N-STRIDE
// （*4/*8/*16 的 shl-vs-lea，探针 10 形全败）+ 少量寄存器名/中转（N7/N18a）
// + difflib 错位，语义完成。
// [J9MgrTimerCLeagueMatch.V1 2026-09-11 终局] 本轮两旋钮：①三处 `(&m_teamA)[t]`
// →`m_teams[t]`（读取 GetDailyBadge 的 user 寻址）⇒ our 989→983、行差 394/387→
// 346/333（改善）**但 gap 990-989=1→990-983=7 劣**（指令数硬指标）⇒ 按「四数不劣
// + 不加距」回滚，保持 S180 取优形；②`int type;` → `int type = 0;`（P328⑤ 反向）
// ⇒ our 990、行差 388（各 +1）劣，回滚。gap=+7 缺口的 ORIG-only 主体已逐形归因 =
// 0x0855f0xx 段 synth_mult copy 链（`mov %ecx,%eax`×14 / `mov %eax,%ecx`×13 /
// `lea (%edx,%eax,1),%eax`×17）+ N125（`lea (,%eax,4)`×8），同 OnCompleteLoad 探针
// 网络结论【不可翻】。p167：45/45 调用点全等，MISSING/EXTRA 仅 C1/C2·D1/D2 同址别名对。
#include "CLeagueMatch/OnReward.inc"
{
    // [G9WorldTimerLeague 2026-09-10 复核] 成员 disp / 立即数多重集扫描
    // （ORIG 切片 vs fresh .o）：唯一 OURS-only {0x10:3, 0x80:3, 0x1c:3}
    // 与 ORIG-only {0xffffff80:3} 全部落在 `add $0x10` / `sub $0xffffff80`
    // （= add 0x80，ORIG 0x0855ed34 / 0x0855ee4e / 0x0855ef48 三处）与
    // stride-0x1c 强度归约链上 ⇒ N125/N4（4.1 lea+立即数拆分 vs 4.4 shl/add）
    // + N107 族，无语义缺口；p167 CLEAR（仅 C1/C2·D1/D2 同址别名对）。
    // ---- 结果包（ORIG 0x0855eb37 call W Packet_PvPResult::C1Ev 0x08568dc6，
    //       重建侧已有该 W 符号（PacketHeader(0xfa5,0x80)），改栈对象真 ctor；
    //       原手写字段展开含 reversed1/m_connNo 额外清零，属发明语义已删——
    //       ORIG ctor 仅设包头，不清该 2 字节） ----
    Packet_PvPResult res;

    // ORIG asm 统一寻址公式：team 块基 = pkt + 0x10 + 56*t，块内
    //   徽章值 @ 0x00 + i + 3*j（`movzwl 0x10(%ebx,%eax,2)` 读 PlayerInfo
    //   m_badges[i] → `mov %bl,(%eax)` 单字节写）
    //   新获旗标 @ 0x0c + i + 3*j（`movb $0x0,0xc(%eax)`）
    // 即 Packet_PvPResult::Team::m_dailyBadge[j][i] / m_newFlag[j][i]。
    // 1) 初始：两支队伍每日徽章快照写 m_dailyBadge[j][i]，同队旗标区清零。
    // [b28d-solo-CLeagueMatch·S408] P191：ORIG 循环变量 t 的 init 段有**两条**
    // `movl $0x0,-0x3c(%ebp)`（声明初始化 + for 初始化）⇒ 原码为
    // `int t = 0;` 前置声明 + `for (t = 0; …)`；三处 t 循环共用同一槽。
    int t = 0;
    for (t = 0; t <= 1; ++t) {
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            for (int i = 0; i <= 2; ++i) {
                res.m_team[t].m_dailyBadge[j][i] =
                    static_cast<char>(m_teams[t].m_players[j].m_badges[i]);
                res.m_team[t].m_newFlag[j][i] = 0;
            }
        }
    }

    // 2) 各分支记徽章（draw→type1 / A 队胜→type2 / 其余→type0）。
    // [b28d-solo-CLeagueMatch·S408] P0：ORIG 三分支各自 `movl $K,-0x38(%ebp)`、
    // 写包头时 `mov -0x38(%ebp),%eax; mov %al,-0xca(%ebp)` ⇒ 原码是**无初始化
    // 声明的 int type**（三分支全赋值）且 m_winType 直接取 type，无 char winType
    // 三目物化；m_eloK/m_playCount 亦无具名局部（ORIG 直读全局/静态后写包）。
    int type;
    if (draw) {
        type = 1;
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            res.m_team[0].m_newFlag[j][1] = 0;   // 自队蓝章清零
            addBadge(&m_teamA.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res.m_team[0].m_newFlag[j][2], false);
            res.m_team[1].m_newFlag[j][1] = 0;
            addBadge(&m_teamB.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res.m_team[1].m_newFlag[j][2], false);
        }
    } else if (winTeam == 1) {
        type = 2;
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            addBadge(&m_teamA.m_players[j], static_cast<ENUM_BADGE_TYPE>(1),
                     &res.m_team[0].m_newFlag[j][1], false);
            res.m_team[0].m_newFlag[j][2] = 0;
            res.m_team[1].m_newFlag[j][1] = 0;
            addBadge(&m_teamB.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res.m_team[1].m_newFlag[j][2], false);
        }
    } else {
        type = 0;
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            res.m_team[0].m_newFlag[j][1] = 0;
            addBadge(&m_teamA.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res.m_team[0].m_newFlag[j][2], false);
            addBadge(&m_teamB.m_players[j], static_cast<ENUM_BADGE_TYPE>(1),
                     &res.m_team[1].m_newFlag[j][1], false);
            res.m_team[1].m_newFlag[j][2] = 0;
        }
    }

    // 写包头数据字段（asm：elo_k2_@+0x0a 4B、winType@+0x0e 1B、playCount@+0x0f 1B）。
    // [b28d-solo-CLeagueMatch·S408] P0 内联：ORIG 0x0855f0xx 段三条均无局部槽往返。
    res.m_eloK = PvPGlobalEnvironmentVariable::school_match_elo_k2_;
    res.m_winType = static_cast<char>(type);
    res.m_playCount = nPlayerCountPerOneTeam_;

    // 3) 两队 schoolNo 表（intsBase=pkt+8，elem=(t*14+i+8)，per-team stride 56B）与
    //    school 排他检查（同校不算对手）及绿章提示。
    for (t = 0; t <= 1; ++t) {
        // [b28d-solo-CLeagueMatch·S408] P0/P183/P191：ORIG 0x0855efd3/0855efda 双
        // `movl $0x0,-0x20(%ebp)`（i 声明初始化 + 循环 1 初始化）后三个 i 循环复用
        // 同一 -0x20 槽；全程无 TeamInfo&/PlayerInfo&/schools/mySchool/fmt 具名局部
        // （每访问点重算 this+t*0x70+i*0x1c，ORIG 0x0855f057/f131/f2d5 三段实证）。
        int i = 0;
        for (i = 0; i < (int)nPlayerCountPerOneTeam_; ++i) {
            res.m_team[t].m_schoolNo[i] = m_teams[t].m_players[i].m_schoolNo;
        }
        for (i = 0; i < (int)nPlayerCountPerOneTeam_; ++i) {
            if (_getSameSchool(res.m_team[t].m_schoolNo, i,
                                (int)nPlayerCountPerOneTeam_,
                                res.m_team[t].m_schoolNo[i])) {
                // [SweepLeagueMatch2·P59/P222] ORIG 物理块序（855f1ac extra=1 →
                // 855f232 extra=0 → 855f23c 绿章）⇒ 外层正极性 if(sameSchool)
                // 使 addBadge 两臂落前、绿章落尾；内层 `!=0||!=0` 正极性短路。
                if (res.m_team[t].m_newFlag[i][1] != 0 ||
                    res.m_team[t].m_newFlag[i][2] != 0) {
                    addBadge(&m_teams[t].m_players[i], ENUM_BADGE_TYPE_0,
                             &res.m_team[t].m_newFlag[i][0],
                             true);
                } else {
                    addBadge(&m_teams[t].m_players[i], ENUM_BADGE_TYPE_0,
                             &res.m_team[t].m_newFlag[i][0],
                             false);
                }
            } else {
                res.m_team[t].m_newFlag[i][0] = 0;
                if (res.m_team[t].m_newFlag[i][1] != 0 ||
                    res.m_team[t].m_newFlag[i][2] != 0) {
                    // [b28d-solo-CLeagueMatch·S408] P224/P296/P168a②：ORIG
                    // 0x0855f303（`movzbl; xor $1; test; jne 0855f50a`）、
                    // 0x0855f312（`mov (%eax),%eax; test; je 0855f50d`）、
                    // 0x0855f34a（`call getCurCharacR; test; sete; test; jne 0855f510`）
                    // 是**三条独立负向守卫**，各自落**独立 nop+jmp 中继岛**
                    // （0855f50a/50d/510 三个岛）⇒ 原码为 `if(!flag) continue;` /
                    // `if(user==0) continue;` / `if(getCurCharacR()==0) continue;`
                    // （正向 && 链反产 je 直落、无岛、无 xor）。
                    if (!m_teams[t].m_players[i].m_flag)
                        continue;
                    if (m_teams[t].m_players[i].m_user == 0)
                        continue;
                    if (m_teams[t].m_players[i].m_user->getCurCharacR() == 0)
                        continue;
                    char textBuf[0x100] = { '\0' };   // P4：ORIG rep stos 内联清零
                    // [S408] P0：ORIG 0x0855f3c9..0x0855f3e6 findString 返回
                    // %eax 直压 0x8(%esp)（无 fmt 具名局部槽）。
                    OS_API::snprintf(
                        textBuf, 0xff,
                        g_scriptStringManager_.findString(
                            4, "school_green_badge_msg", (bool*)0),
                        1);
                    PacketGuard pg;
                    pg.put_header(0, 0xc);
                    pg.put_byte(0);
                    pg.put_short(0);
                    pg.put_byte(0);
                    pg.put_int((int)strlen(textBuf));
                    pg.put_str(textBuf, (int)strlen(textBuf));
                    pg.finalize(true);
                    m_teams[t].m_players[i].m_user->Send(pg);
                }
            }
        }
        // 回填实时徽章计数。ORIG 0x0855f539..0x0855f6da：条件寻址固定
        // this+i*0x1c+0x98(m_flag)/0x94(m_user) = **teamB** 槽，与 t 无关；
        // 读取 user 为 this+i*0x1c+t*0x70+0x24（含 t）；GetDailyBadge 为 3 条
        // 独立调用（$0x0/$0x1/$0x2 立即数直传，非循环，P0）。
        for (i = 0; i < (int)nPlayerCountPerOneTeam_; ++i) {
            // [SweepLeagueMatch2·P59] ORIG 0x0855f539(m_flag) / 0x0855f561(m_user)
            // 两守卫同落 855f6dc 岛（正向 `test; je` 短路）⇒ 正向包裹。
            if (m_teamB.m_players[i].m_flag != 0 &&
                m_teamB.m_players[i].m_user != 0) {
                res.m_team[t].m_dailyBadge[i][0] =
                    static_cast<char>((&m_teamA)[t].m_players[i].m_user->GetDailyBadge(
                        static_cast<ENUM_BADGE_TYPE>(0)));
                res.m_team[t].m_dailyBadge[i][1] =
                    static_cast<char>((&m_teamA)[t].m_players[i].m_user->GetDailyBadge(
                        static_cast<ENUM_BADGE_TYPE>(1)));
                res.m_team[t].m_dailyBadge[i][2] =
                    static_cast<char>((&m_teamA)[t].m_players[i].m_user->GetDailyBadge(
                        static_cast<ENUM_BADGE_TYPE>(2)));
            }
        }
    }

    // 4) Elo 结算并回写个人分 + accId 表（elem=(t*14+i+12)）。
    // [b28d-solo-CLeagueMatch·S408] ORIG 0x0855f70b/f712 是单段 `movl $0x0,-0x44/-0x40`
    // （float 数组的 4.1 零初始化形）；4.4 对 `float ratings[2]={0.0f,0.0f}` 恒发
    // 二段 `mov $0,%eax; mov %eax,-N`（P232 扩展）⇒ 用等宽 union int 视角写零。
    union { float f[2]; unsigned int u[2]; } ratings;
    ratings.u[0] = 0;
    ratings.u[1] = 0;
    for (unsigned int i = 0; i < (unsigned int)nPlayerCountPerOneTeam_; ++i) {
        for (t = 0; t <= 1; ++t) {
            ratings.f[t] = m_teams[t].m_players[i].m_rating;
        }
        CEloRating::calc(ratings.f[0], ratings.f[1],
                         static_cast<ENUM_ELO_RATING_SYSTEM>(type),
                         PvPGlobalEnvironmentVariable::school_match_elo_k_);
        for (t = 0; t <= 1; ++t) {
            // [b28d-solo-CLeagueMatch·S408] 真语义 bug 修正（多余守卫，P140/P171）：
            // ORIG 0x0855f7d3..0x0855f819 只 `movzbl +0x18(m_flag); test; je`
            // 后**直接**取 +0x24(m_user) 调 UpdateRating，**无 m_user!=0 检查**
            // ⇒ 删多余守卫照抄 ORIG 行为。
            if (m_teams[t].m_players[i].m_flag != 0)
                m_teams[t].m_players[i].m_user->UpdateRating(ratings.f[t]);
            res.m_team[t].m_accId[i] = m_teams[t].m_players[i].m_accId;
        }
    }

    // 5) 上报：单人走消息队列流（0x136/-1 + 二进制），多人直发学校服务器代理。
    // [SweepLeagueMatch2·P59] ORIG 物理块序 SendPacket(855f8c2) 先于
    // StreamPool::Acquire(855f8e4) ⇒ 外层负极性 if(count!=1) 使直发臂落前。
    if (nPlayerCountPerOneTeam_ != 1) {
        GlobalData::s_school_proxy->SendPacket(reinterpret_cast<char*>(&res), 0x80);
    } else {
        // [b28d-solo-CLeagueMatch·S408] P0：ORIG 0x0855f8e4..0x0855f8fb 的
        // Acquire 返回值 %eax 直压 0x4(%esp)（无 st 具名局部槽）。
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("MatchingSystem.cpp", 0x4a8), true);
        // ORIG 每次写经显式 operator*() 调用（同 TimerDispatcher 代理链形）；
        // 直 deref/shim 会丢 2 处 operator*() call（P167 缺失）。
        guard.operator*()->operator<<((int)0x136);
        guard.operator*()->operator<<((int)-1);
        guard.operator->()->put_binary(&res, 0x80);
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    }
}

// ORIG T 0x0855e22c：向全体成员广播加载完成通知（0x81 包，携带 unique id）。
// [b28d-solo-CLeagueMatch] DIFF 97/95·ndiff 22·N-STRIDE（S217 新鲜 .o 复验）：
// 残差 = i*0x1c 的 shl-vs-lea + 0x70 族乘法展开序差（ORIG 先 `shl $4`(t*16)
// 再 `lea(,%x,8);sub`，4.4 先算 i*7 再 shl），逐条语义等价；p167 9/9 CLEAR。
// [J9MgrTimerCLeagueMatch.V1 2026-09-11 终局 · gap=+2 缺口定位] 本件 ORIG 唯一
// ORIG-only 段 = 0x0855e2b7/0x0855e2b9 `mov %ecx,%eax; mov %eax,%ecx`（t 的纯
// 冗余寄存器往返，删去后逐条等价）。4.4.7 探针穷举（/tmp/probe_ocl{1,2,3}：
// 70 源形——数组/指针 deref/`*(TI*)((char*)p+off)`/sizeof/char* 偏移链/两层引用/
// 具名中间量/register/unsigned·cast 索引/上界写法/加法序）恒产
// `mov %edx,%eax; shl $0x4,%eax`（无往返）；追加 `#pragma GCC target`
// (tune=i386/i486/pentium/pentiumpro · arch=i386/i486/pentiumpro · tune=generic)
// 8 形 + `#pragma GCC optimize("strength-reduce"/"O1"/"web")` 3 形 + cc1plus
// **4.4.6 后端** A/B（头集 4.4.6 + i486，独立 .o，逐件读数与本 TU 全同）亦全败
// ⇒ 该 2 条为 ORIG 编译器（4.1 系）synth_mult 的 copy 链产物，4.4 不可翻【已试尽】。
// p167 9/9 CLEAR：MISSING `PacketGuardC2Ev`×1/`D2Ev`×2 ≡ EXTRA `C1Ev/D1Ev`
// （nm ORIG 实证同址别名 0x0858dd4c / 0x0858de80）⇒ 非语义缺口。
#include "CLeagueMatch/OnCompleteLoad.inc"
{
    PacketGuard pkt;
    pkt.put_header(0, 0x81);
    pkt.put_short(user->get_unique_id() & 0xffff);
    pkt.finalize(true);
    // [b28d-solo-CLeagueMatch] P183/P0：ORIG 无 TeamInfo&/PlayerInfo& 引用缓存槽
    // （Ghidra 仅 t/i/user 三局部）；以 (&m_teamA)[t] 数组化寻址 + user 具名局部
    // （ORIG -0x10）复现，并消 4.4 的引用槽。
    for (int t = 0; t <= 1; ++t) {
        for (int i = 0; i < 4; ++i) {
            CUser* member = m_teams[t].m_players[i].m_user;
            if (m_teams[t].m_players[i].m_flag != 0 && member != 0)
                member->Send(pkt);
        }
    }
}

#include "CLeagueMatch/CheckJoinable.inc"
{
    if (room->get_player_count() >= (int)(nPlayerCountPerOneTeam_ * 2))
        return 4;
    if (m_teamA.size() + m_teamB.size() > (int)(nPlayerCountPerOneTeam_ * 2))
        return 4;

    switch (m_state) {
    case 0: {
        // [b28c-21 代落 C28L25 spec] ORIG `call _CheckSchool; xor $1,%eax;
        // test %al,%al; je` = bool callee 裸形（P168a①）；int callee 恒 sete。
        if (!_CheckSchool(room, user))
            return 4;
        return 0;
    }
    case 3:
        return 0;
    case 1:
    case 4:
    case 5:
        return 4;
    default:
        return 4;
    }
}

#include "CLeagueMatch/CheckSeat.inc"
{
    if (seat == 2) return 0;
    return 1;
}

// [b28d-solo-CLeagueMatch] DIFF 202/202·真语义 bug 已修 + N-STRIDE：①段位窗口边界
// 取反真 bug（ORIG 0x0855fbc0..fbf3 `jg` 跳过 return 0 ⇒ return false 条件是
// `self∈[12,14] && other<=10`，原实现写成 `other>10`）——改 `<=10` 后 jg/jle
// 差异同步消失；②i 复用同一槽 + `int i=0` 双初始化 + 具名 diff（P191/P0）；
// 残差 12 处为 stride 0x1c 的 shl-vs-lea（N-STRIDE：4.1 `lea 4;lea 8;sub` vs
// 4.4 `shl 2;lea 8;sub`，S180 探针 /tmp/b28d_cm/probe_stride.cpp 11 形 +
// probe2.cpp 7 索引类型 + 4.4.6 后端 + 8 旗标全败）；p167 CLEAR（4/4）。
// [J9MgrTimerCLeagueMatch.V1 2026-09-11 终局] 本轮两旋钮：①`int i` → `unsigned int i`
// （P290）⇒ ORIG-only/OURS-only 134→140 劣（6 处，循环比较由 setle 翻 setbe），已
// 回滚；②本类 `m_teamA.m_players[i]` 三循环 → union 等宽视角 `m_teams[0]`（P235）
// ⇒ 202/202、134/134 逐项全同（零差），已回滚保持历史形。语义轴已闭合（真边界 bug
// `other<=10` 已修），残差纯 N125【不可翻】。
#include "CLeagueMatch/CheckMatchable.inc"
{
    // ORIG 汇编对 other 直接按对象前部偏移读其第一队 PlayerInfo 槽
    // （this+slot*0x1c+0x24/0x28/0x2c），依赖 IMatch 派生类共享布局。
    // ORIG 无引用缓存：每次成员访问都重算 base+i*0x1c（每处 7 条寻址指令），
    // 且胜点比较是 MAX 宏形态 —— getPVPResultR()->m_pvpGrade 被求值两次；
    // 差值判定用 abs()（单临时槽 sar/xor/sub 展开），不得手写三遍表达式。
    int maxWinSelf = 0;
    int maxWinOther = 0;
    // [b28d-solo-CLeagueMatch] P0/P191：ORIG 0x0855fa1b/fa22/fa29/fa30 四清零
    // = maxWinSelf/maxWinOther + i 双初始化（`int i = 0;` + `for (i = 0;…)`）；
    // 三个循环共用同一 i 槽 -0x14（ORIG 0x0855faf8/fc1d 复用），非各自 for
    // 内声明（4.4 会各占独立槽，实测多 -0x20/-0x18 两槽）。
    int i = 0;

    for (i = 0; i < 4; ++i) {
        if (m_teamA.m_players[i].m_flag != 0 && m_teamA.m_players[i].m_user != 0)
            maxWinSelf =
                (m_teamA.m_players[i].m_user->getPVPResultR()->m_pvpGrade < maxWinSelf)
                    ? maxWinSelf
                    : m_teamA.m_players[i].m_user->getPVPResultR()->m_pvpGrade;
    }
    for (i = 0; i < 4; ++i) {
        if (reinterpret_cast<CLeagueMatch*>(other)->m_teamA.m_players[i].m_flag != 0 &&
            reinterpret_cast<CLeagueMatch*>(other)->m_teamA.m_players[i].m_user != 0)
            maxWinOther = (reinterpret_cast<CLeagueMatch*>(other)
                               ->m_teamA.m_players[i].m_user->getPVPResultR()->m_pvpGrade <
                           maxWinOther)
                              ? maxWinOther
                              : reinterpret_cast<CLeagueMatch*>(other)
                                    ->m_teamA.m_players[i]
                                    .m_user->getPVPResultR()
                                    ->m_pvpGrade;
    }

    // [b28d-solo-CLeagueMatch] 真语义 bug 修正（Ghidra C 对照 0x0855fbc0..fbf3）：
    // ORIG 第一组 `cmpl $0xb,self; jle 第二组 / cmpl $0xe,self; jg 第二组 /
    // cmpl $0xa,other; jg 第二组` ⇒ **跳过** return 0 的条件是 other > 10，
    // 即 return false 当 `self∈[12,14] && other<=10`（Ghidra `(10 < local_1c)`
    // 为跳转条件）；原实现写成 `other > 10` 恰把边界取反（真语义 bug）。
    if ((maxWinSelf >= 12 && maxWinSelf <= 14) && maxWinOther <= 10)
        return false;
    if ((maxWinOther >= 12 && maxWinOther <= 14) && maxWinSelf <= 10)
        return false;
    // [b28d-solo-CLeagueMatch] P0：ORIG 0x0855fc07 把 abs 结果写具名槽 -0x10
    // （mov %edx,-0x10(%ebp); sub %eax,-0x10(%ebp)）⇒ 具名 diff 局部。
    int diff = abs(maxWinSelf - maxWinOther);
    if (diff > 4)
        return false;

    for (i = 0; i < 4; ++i) {
        for (int b = 0; b < 4; ++b) {
            if (m_teamA.m_players[i].m_flag != 0 &&
                reinterpret_cast<CLeagueMatch*>(other)->m_teamA.m_players[b].m_flag != 0 &&
                m_teamA.m_players[i].m_schoolNo ==
                    reinterpret_cast<CLeagueMatch*>(other)->m_teamA.m_players[b].m_schoolNo)
                return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// 非 IMatch 方法
// ---------------------------------------------------------------------------

#include "CLeagueMatch/SetMatchAble.inc"
{
    bMatchable_ = able;
}

#include "CLeagueMatch/SetPlayCountPerOneTeam.inc"
{
    nPlayerCountPerOneTeam_ = static_cast<unsigned char>(n);
}

// ORIG 虚槽布局：GetSchoolNo 为派生新增第 37 槽（ORIG vtable +0x90），
// GetIndentification 的覆盖体即经 vtable+0x90 虚调本函数。
// [b28d-solo-CLeagueMatch] DIFF 32/32·N-STRIDE：残差全为 stride 0x1c 的
// shl-vs-lea（2 访问点），语义等价，结构性豁免。
#include "CLeagueMatch/GetSchoolNo.inc"
{
    for (int i = 0; i <= 3; ++i) {
        if (m_teamA.m_players[i].m_flag != 0)
            return m_teamA.m_players[i].m_schoolNo;
    }
    return 0;
}

// [SweepCLeagueMatch-2·判据㉓] GetIndentification(0x08568834 W) / GetBattleMode
// (0x084ed387 W) / SetBattleMode(0x082743a0 W) / GetPlayCountPerOneTeam
// (0x084ed37b W) 四个 ORIG 弱符号已改类内 inline 定义（CLeagueMatch.h），由
// 使用点 TU 发射 W 副本（P233/P204③）；本 TU 不再外联定义（原 T 绑定与 ORIG
// 的 W 不符）。函数体逐条不变，见头内注释。

// [b28d-solo-CLeagueMatch] AE（TU 口径）：196=196 全等。修法=删 passwd/title
// 引用缓存局部（P183/P0，消 2 个指针槽 -0x24/-0x20）+ S27 PvPTypes.h
// PVP_ROOM_INFO::byte8 uchar→bool（消 uchar→bool 的 test/setne 2 条）。
// p167 调用点差集 CLEAR（39/39）。
#include "CLeagueMatch/_MakeRoom.inc"
{
    PacketGuard pkt;
    pkt.put_header(1, 0x35);
    PvP_Room* pvp = G_CGameManager()->GetPvp();
    if (pvp == 0) {
        pkt.put_byte(0);
        pkt.put_byte(4);
        pkt.finalize(true);
        user->Send(pkt);
        return 0;
    }

    // [b28d-solo-CLeagueMatch] P183/P0：ORIG 0x0855d5xx 每处重算
    // `mov 0x8(%ebp),%eax; add $0x14/$0x8,%eax`（无 passwd/title 引用缓存槽）⇒ 删引用
    // 局部改直接成员访问，消 2 个指针槽（-0x24/-0x20）。
    int err = pvp->create_room(user, m_roomInfo.byte4,
                               const_cast<char*>(m_roomInfo.title.c_str()), m_roomInfo.short6,
                               m_roomInfo.byte8 != 0, m_roomInfo.passwd.c_str(),
                               static_cast<int>(m_roomInfo.passwd.length()),
                               static_cast<PVP_BATTLE_MODE>(2));
    if (err >= 1) {
        pkt.put_byte(0);
        pkt.put_byte(err);
        pkt.finalize(true);
        user->Send(pkt);
        // [J9MgrTimerCeraItemLogin.V1CLeagueMatch 2026-09-11 · §6.5 日志文本契约]
        // ORIG 该串实字节（二进制文件偏移 0xb7a2c8 / 0xc5a23c 两份一致副本）为
        // EUC-KR `create_room fail: PVP \xbd\xbd\xb7\xd4\xb9\xdd\xc8\xaf ID[%d]\n`
        // （"슬롯반환" = U+C2AC U+B86F U+BC18 U+D658 的 cp949 编码，8 字节）。
        // 源码原写 UTF-8（ec 8a ac eb a1 af eb b0 98 ed 99 98，12 字节）⇒ 发射
        // 字节与 ORIG 不同、AE 归一化不可见（§6.5 盲区）⇒ 以 \xNN 转义复现
        // ORIG 实字节（串内容变化，函数指令/codegen 不受影响）。
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__, 0x1fe,
            "create_room fail: PVP\xbd\xbd\xb7\xd4\xb9\xdd\xc8\xaf ID[%d]\n",
            pvp->get_index());
        G_CGameManager()->PutPvp(pvp);
        return 0;
    }

    pkt.clear();
    pkt.put_header(0, 2);
    pkt.put_byte(1);
    pkt.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&pkt), 1);
    pkt.finalize(true);
    user->Send(pkt);

    pkt.clear();
    pkt.put_header(0, 0x29);
    pkt.put_short(1);
    pvp->make_room_info(reinterpret_cast<char*>(&pkt));
    pkt.finalize(true);
    G_GameWorld()->send_all(pkt);
    G_GameWorld()->goto_pvp(user);
    return pvp;
}

// [b28d-solo-CLeagueMatch] DIFF 132/132·ndiff 24·N-STRIDE+N7（S217 新鲜 .o
// 复验；x87 比较段已产 ORIG 的 `fucompp;fnstsw;sahf/test;sete` 形，S180 判词
// 里的 fucomip/seta 系 stale 链接二进制假象）：foundMin 改
// int（ORIG -0x20 `cmpl $0`）+ 两循环改嵌套正向 if（P296）后条数 132=132；
// S180 再把 sort 双实参改 P235 视角 m_teams[0]/m_teams[1]，复现 ORIG
// 0x0855da16 的 `add $0x8c` / `add $0x1c` 双独立算址；残差仅剩 stride
// shl-vs-lea 与 x87 比较 lowering（ORIG fucompp/fnstsw/sahf vs 4.4 seta 形），
// 语义等价。
#include "CLeagueMatch/_RegisterWaitingRoom.inc"
{
    // [b28d-solo-CLeagueMatch] ORIG foundMin 为 int（-0x20 `cmpl $0x0,-0x20`）非 bool；
    // 两循环均嵌套正向 if（P296）：m_flag 正向 `test;je`（0x0855d8c7）、
    // get_team==3 正向（0x0855d98a），消 continue 的 jmp+nop 中继岛。
    int foundMin = 0;
    float minRating = 0.0f;
    for (int i = 0; i < 4; ++i) {
        if (m_teamA.m_players[i].m_flag != 0) {
            if (foundMin == 0) {
                minRating = m_teamA.m_players[i].m_rating;
                foundMin = 1;
            }
            if (m_teamA.m_players[i].m_rating < minRating)   // asm fucompp：小于最小值则更新
                minRating = m_teamA.m_players[i].m_rating;
        }
    }
    minRating -= 1.0f;

    for (int seat = 0; seat <= 7; ++seat) {
        if (room->get_team(seat) == 3) {
            CUser* seatUser = reinterpret_cast<CUser*>(room->get_user_seat(seat));
            for (int i = 0; i < 4; ++i) {
                if (m_teamA.m_players[i].m_user == seatUser && seatUser != 0)
                    m_teamA.m_players[i].m_rating = minRating;
            }
        }
    }

    std::sort<PlayerInfo*, bool (*)(PlayerInfo const&, PlayerInfo const&)>(
        m_teams[0].m_players, m_teams[1].m_players, CompareRating);  // ORIG weak sort 实例
        // [b28d-solo-CLeagueMatch] P235：ORIG 0x0855da16 两实参各自独立算址
        // （this+0x1c / this+0x8c）；写 m_teamA.m_players+4 会让 4.4 先算基址再
        // lea 0x70 ⇒ 用等宽数组视角 m_teams[0]/m_teams[1] 复现 ORIG 双独立地址。
    pWaitingRoomList_->AddRoom(this);                               // IRoomList vtable+0x00
}

#include "CLeagueMatch/_BackupRoomInfo.inc"
{
    room->get_room_info(&m_roomInfo);
}

// [b28d-solo-CLeagueMatch] AE（TU 口径）：69=69 全等。修法=P225 `while(cond){return X;}`
// 复现 ORIG 尾 `cmp %eax,%edx; setne %al; test %al,%al; je` 的 bool 四段物化
// （三目/if 直跳形恒被 4.4 折成 `jne`，探针 /tmp/b28d_cmatch 两形实测：三目
// ndiff 8、bool 局部占槽反劣 13、while+break 归零）；槽序按 P0 对齐
// （emptySeats/X/i/user，X=logArg0 复现 -0x14 具名局部，删 school 局部改两次
// GetSchoolNo P48）。
#include "CLeagueMatch/_CheckMatchable.inc"
{
    int emptySeats = 0;
    // [b28d-solo-CLeagueMatch] P0：ORIG -0x14 具名局部（恒 0）作 logFormat 首个 %d
    // 实参（0x0855dad6 `mov -0x14(%ebp),%eax; mov %eax,0x14(%esp)`）；删除 school
    // 局部改两次 GetSchoolNo（P48 O0 无 CSE），槽序 emptySeats/X/i/user 与 ORIG 全等。
    int logArg0 = 0;
    for (int i = 0; i <= 7; ++i) {
        if (room->get_team(i) == 3) {
            ++emptySeats;
            continue;
        }
        CUser* user = reinterpret_cast<CUser*>(room->get_user_seat(i));
        if (user == 0)
            continue;
        if (user->GetSchoolNo() == 0) {
            LogManager::logFormat(
                1, "MatchingSystem.cpp",
                __PRETTY_FUNCTION__, 599,
                "(%d) found a different school no(%d)", logArg0, user->GetSchoolNo());
            return 0x16;
        }
    }
    while (m_teamA.size() - emptySeats != (int)nPlayerCountPerOneTeam_) {
        return 0x16;
    }
    return 0;
}

#include "CLeagueMatch/_PullOutRoom.inc"
{
    // OnSetPVPSeatState 返回 1 时成员留在房内。
    if (!room->OnSetPVPSeatState(user, seat,
                                 static_cast<ENUM_SEAT_STATE>(0xfe),
                                 static_cast<ENUM_WALKOUT_TYPE>(2))) {
        room->reset();
        G_CGameManager()->PutPvp(room);
    }
}

// [b28d-solo-CLeagueMatch] DIFF 85/83·N104（S180 复验，p167 15/15 CLEAR）：
// EH 尾块相对序（ORIG 正常出口块在函数末、unwind 居中；块内容逐条全等，
// §6.2 口径语义闭合）。
#include "CLeagueMatch/_PutInRoom.inc"
{
    // [G9WorldTimerLeague·CLMSub 2026-09-10] N104 现场复现（ndiff 19→**0**，
    // 85/85 全等；TU ae 62→63 / diff 17→16）：ORIG 0x0855d42a 段**两个出口分支
    // 各自**带 `mov -0x1c(%ebp),%ebx; jmp <尾块>`（4.1 不做分支尾合并），原
    // `if/else + 落尾 return` 形使 4.4 只在尾块头发一条 mov 且 else 侧直落 ⇒
    // gap 2。改「删 else、两分支各自 `return seatNo;`」后 4.4 产同形双出口。
    PacketGuard pkt;
    int seatNo = 0;
    int err = room->join_room(user, seatNo, observer);
    if (err > 0) {
        pkt.put_header(1, 0x36);
        pkt.put_byte(0);
        pkt.put_byte(err);
        pkt.finalize(true);
        user->Send(pkt);
        return seatNo;
    }
    room->make_seat_info(reinterpret_cast<char*>(&pkt), seatNo);
    G_GameWorld()->send_all(pkt);
    G_GameWorld()->goto_pvp(user);
    return seatNo;
}

// [b28d-solo-CLeagueMatch] AE（TU 口径）：79=79 全等。修法=P223② 显式提前
// `continue`（同层多 continue 复现 ORIG 0x0855e477/e47a/e47d 三个独立
// nop+jmp 中继岛；else-if 合并形被 4.4 jump-thread 少 2 岛，gap 2→0）。
#include "CLeagueMatch/_ReturnTeam.inc"
{
    // [b28d-solo-CLeagueMatch] P0：ORIG 槽序 user(-0x18)/madeRoom(-0x14)/seat(-0x10)
    // ⇒ user 声明于函数头并置 0（有槽往返=具名局部），循环内赋值。
    CUser* user = 0;
    PvP_Room* madeRoom = 0;
    for (unsigned int seat = 0; seat <= 7; ++seat) {
        user = reinterpret_cast<CUser*>(room->get_user_seat(static_cast<int>(seat)));
        if (user == 0)
            continue;
        int seatTeam = room->get_team(static_cast<int>(seat));
        if (team == seatTeam) {
            // [b28d-solo-CLeagueMatch] P223②：ORIG 0x0855e47a/e47d 两个独立
            // nop+jmp 中继岛 = 同层多 `continue`（提前 continue 形），
            // else-if 合并形被 4.4 jump-thread 少 2 岛。
            if (team != 2)
                continue;
            room->OnSetPVPSeatState(user, static_cast<int>(seat),
                                    static_cast<ENUM_SEAT_STATE>(1),
                                    static_cast<ENUM_WALKOUT_TYPE>(2));
            continue;
        }
        if (except != 0 && user == except)
            continue;
        _PullOutRoom(user, room, static_cast<int>(seat));
        if (madeRoom == 0)
            madeRoom = _MakeRoom(user);
        else
            _PutInRoom(user, madeRoom, true);
    }
    m_state = 0;
}

// [b28d-solo-CLeagueMatch] DIFF 95/93·ndiff 22·N-STRIDE（S217 新鲜 .o 复验）：
// 残差 = i*0x1c 的 shl-vs-lea + 0x70 族乘法展开序差（同 OnCompleteLoad），
// 语义等价；p167 8/8 CLEAR。
// [J9MgrTimerCLeagueMatch.V1 2026-09-11 终局] gap=+2 缺口 = ORIG 0x0855e2b7 同族
// `mov %ecx,%eax; mov %eax,%ecx` 冗余 copy 链（同一循环体两份各 ×2，共 2 条差），
// 70 形/pragma target 8 形/pragma optimize 3 形/4.4.6 后端 A/B 全败（证据同
// OnCompleteLoad）【不可翻】；p167 8/8 CLEAR（C1/C2·D1/D2 同址别名）。
#include "CLeagueMatch/_SendMatchStatePacket.inc"
{
    PacketGuard pkt;
    pkt.put_header(0, 0xaf);
    pkt.put_byte(state);
    pkt.finalize(true);
    // [b28d-solo-CLeagueMatch] P183/P0：ORIG 无 TeamInfo& 引用槽（t/i/user 三局部，
    // user 具名缓存 -0xc，寻址 this+i*0x1c+t*0x70+0x24）⇒ 数组化访问复现。
    for (int t = 0; t < teams; ++t) {
        for (int i = 0; i <= 3; ++i) {
            CUser* member = m_teams[t].m_players[i].m_user;
            if (m_teams[t].m_players[i].m_flag != 0 && member != 0)
                member->Send(pkt);
        }
    }
}

// [b28d-solo-CLeagueMatch] AE：TU 口径 identical/AE（25=25 全等；game_func_report
// 单函数口径多 1 条循环对齐 nop，§6.1/N112 已在 TU 口径剥离）。
#include "CLeagueMatch/_getSameSchool.inc"
                                 int count, unsigned int schoolNo)  // ORIG T 0x0855e7a8
{
    for (int i = 0; i < count; ++i) {
        if (i == selfIdx)
            continue;
        if (schools[i] == schoolNo)
            return 1;
    }
    return 0;
}

#include "CLeagueMatch/_GetOppositeTeam.inc"
{
    /* ORIG @0855e4a1 比较链 cmp $1,%eax;je/cmp $2;je/jmp def = 4.1 switch
     * 下探形【技巧34，OnlinePreliminary 同法 IDENTICAL 已验】。ORIG default
     * 块尾多一条 `mov 0xc(%ebp),%eax`(@0855e4be) 重载：4.4 沿 cfg 传播 eax
     * 免载，probe(register 副本/asm("eax")+clobber)均不可逼出，N18a 类 */
    switch (team)
    {
    case 1:
        return 2;
    case 2:
        return 1;
    default:
        break;
    }
    return team;
}

#include "CLeagueMatch/_CheckSchool.inc"
{
    if (user->GetSchoolNo() == 0)
        return 0;
    int emptySeats = 0;
    for (int i = 0; i <= 7; ++i) {
        if (room->get_team(i) == 3)
            ++emptySeats;
    }
    if ((int)(m_teamA.size() - emptySeats) >= (int)nPlayerCountPerOneTeam_)
        return 0;
    return 1;
}

// [b28d-solo-CLeagueMatch] AE（TU 口径）：207=207 全等。修法=P305 的 S18 规避形
// `switch((int)badgeType)`：既避免 -fstrict-enums 删越界 case（ENUM_BADGE_TYPE
// 仅定义 0，曾删 case 2 ⇒ p167 MISSING 各 1），又复现 ORIG 0x0855e90f/914/919
// 的「形参载入 %eax + 寄存器比较链」（P251/P286；if 链恒折 `cmpl $K,mem` 每段
// 重读内存，探针 /tmp/b28d_cmatch/probe4.cpp sw3 vs if3 实证）；p167 25/25 CLEAR。
#include "CLeagueMatch/addBadge.inc"
                           unsigned char* msgFlag, bool extra)   // ORIG T 0x0855e7ee
{
    // [b23S3C] 直返形（ORIG 各 guard return-0 body fall-through）；flag 判定
    // 为 `== 1`（ORIG xor$1：flag∉{0,1} 值域下 ==1 才过，`!flag` 语义差）；
    // 红蓝合计去 sum 具名局部（ORIG 无槽：movswl/cwtl/lea 直比 cmp $9,%eax）；
    // count 双句式 `int count=0; if(extra)++count`（ORIG `movl $0; cmpb; addl$1`
    // 分支岛非三目物化）；三分支各复制完整 findString+snprintf call 序列
    // （ORIG 三独立岛，非选串指针汇聚）；值表达式 (int)*msgFlag+count 右→左
    // 求值先于 findString（ORIG ebx 先算）。
    // N96 清绑：字段表达式直读（P0）；xor$1 单段形复测。
    if ((info->m_flag ^ 1) != 0)
        return 0;
    if (info->m_user == 0)
        return 0;
    if (info->m_user->getCurCharacR() == 0)
        return 0;

    if (badgeType == 0) {
        // ORIG `mov 0x10(%ebp),%edx` 传形参变量本身（非 $0 立即数）。
        if (info->m_user->GetDailyBadge(badgeType) > 9)
            return 0;
    } else {
        if (info->m_user->GetDailyBadge(static_cast<ENUM_BADGE_TYPE>(1)) +
            info->m_user->GetDailyBadge(static_cast<ENUM_BADGE_TYPE>(2)) > 9)
            return 0;
    }

    info->m_user->AddDailyBadge(badgeType);
    *msgFlag = 1;
    // ORIG：count 初 0，仅 extra 加 1（不读 *msgFlag）。
    int count = 0;
    if (extra)
        ++count;

    char textBuf[0x100] = { '\0' };   // P4：ORIG rep stos 内联清零
    // [b28d-solo-CLeagueMatch] 真语义缺口修正：ENUM_BADGE_TYPE 仅定义值 0，
    // `switch(badgeType)` 在 4.4 被 -fstrict-enums 判值域 {0} 而**删除 case 2**
    // （.o 仅 2×findString/snprintf，ORIG 3×；P167 差集 MISSING 各 1）。
    // 改 `switch((int)badgeType)`（P305 的 S18 规避形）：值域未知 ⇒ 三分支
    // 全保留，且 4.4 产 ORIG 0x0855e90f/914/919 的「形参载入 %eax + 寄存器
    // 比较链」`mov 0x10(%ebp),%eax; cmp $1;je / cmp $2;je / test;je / jmp`
    // （P251/P286；if 链恒折 `cmpl $K,mem` 每段重读内存，探针 /tmp/b28d_cmatch/
    // probe4.cpp sw3 vs if3 实证）。
    switch (static_cast<int>(badgeType)) {
    case 1:
        OS_API::snprintf(textBuf, 0xff,
                         (const char*)g_scriptStringManager_.findString(
                             4, "school_red_badge_msg", (bool*)0),
                         (int)*msgFlag + count);
        break;
    case 2:
        OS_API::snprintf(textBuf, 0xff,
                         (const char*)g_scriptStringManager_.findString(
                             4, "school_blue_badge_msg", (bool*)0),
                         (int)*msgFlag + count);
        break;
    case 0:
        OS_API::snprintf(textBuf, 0xff,
                         (const char*)g_scriptStringManager_.findString(
                             4, "school_green_badge_msg", (bool*)0),
                         (int)*msgFlag + count);
        break;
    }

    PacketGuard pkt;
    pkt.put_header(0, 0xc);
    pkt.put_byte(0);
    pkt.put_short(0);
    pkt.put_byte(0);
    pkt.put_int((int)strlen(textBuf));
    pkt.put_str(textBuf, (int)strlen(textBuf));
    pkt.finalize(true);
    info->m_user->Send(pkt);
    return 1;
}
