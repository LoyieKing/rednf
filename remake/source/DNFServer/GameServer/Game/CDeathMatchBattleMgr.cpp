// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDeathMatchBattleMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// ============================================================================
// df_game_r CDeathMatchBattleMgr（G2-5 批次，PvP_Room +0x69c，尺寸 0x44）
// 逐函数对照 docs/class_func_reports/CDeathMatchBattleMgr.md 与 ORIG 反汇编还原。
// 布局（由 SetParent/IncreaseKillCount/IncreaseDeathCount/OnStart 反汇编推导）：
//   +0x00 PvP_Room* m_pRoom
//   +0x04 int m_killCount[8]     （IncreaseKillCount 用 this+idx*4+4）
//   +0x24 int m_deathCount[8]    （IncreaseDeathCount 用 this+(idx+8)*4+4）
// sizeof == 0x44。
// 返回类型以 ORIG 反汇编/mangling 为准：checkWinner* 为 void（GameStubs 桩的
// int 是错的）；checkGameOver* 返回 0/1（调用点 test %al，声明为 int 与 ORIG
// mov $imm,%eax 形态一致）。
// 跨类调用经权威头（PvP_Room.h/TimerQueue.h/CDeathMatchBattleMgr.h），mangled 名与 ORIG 完全一致。
// ============================================================================

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <utility>

#include "CDeathMatchBattleMgr.h"
#include "PvP_Room.h"
#include "TimerQueue.h"

// cMyTrace 权威声明见 GameTypes.h（经 PvP_Room.h 链包含），本地类已删除（§7）。

// ---- std::pair 排序比较器（ORIG 085df55f T） ----
#include "CDeathMatchBattleMgr/funDeathMatchKillCntCompare.inc"
                                 const std::pair<int, int>& b)
{
    return a.second > b.second;
}

// ---- CDeathMatchBattleMgr（0x44，权威声明见 CDeathMatchBattleMgr.h） ----

// ==== ORIG 085dee22 ====
#include "CDeathMatchBattleMgr/Reset.inc"
{
    memset(m_killCount, 0, sizeof(m_killCount));
    memset(m_deathCount, 0, sizeof(m_deathCount));
}

// ==== ORIG 085df92c（弱符号） ====
#include "CDeathMatchBattleMgr/SetParent.inc"
{
    m_pRoom = room;
}

// ==== ORIG 085dee66 ====
#include "CDeathMatchBattleMgr/OnCreateRoom.inc"
{
    if (roomIdx < 0 || roomIdx > 7)
        return;
}

// ==== ORIG 085deeca ====
#include "CDeathMatchBattleMgr/OnStart.inc"
{
    memset(m_killCount, 0, sizeof(m_killCount));
    memset(m_deathCount, 0, sizeof(m_deathCount));
}

// ==== ORIG 085def0e ====
#include "CDeathMatchBattleMgr/IncreaseKillCount.inc"
{
    if (userIdx >= 0 && userIdx <= 7)
        ++m_killCount[userIdx];
}

// ==== ORIG 085def34 ====
#include "CDeathMatchBattleMgr/IncreaseDeathCount.inc"
{
    if (userIdx >= 0 && userIdx <= 7)
        ++m_deathCount[userIdx];
}

// ==== ORIG 085df524 ====
#include "CDeathMatchBattleMgr/getKillCount.inc"
{
    if (userIdx <= 7)
        return m_killCount[userIdx];
    return 0;
}

// ==== ORIG 085df540 ====
#include "CDeathMatchBattleMgr/getDeathCount.inc"
{
    if (userIdx <= 7)
        return m_deathCount[userIdx];
    return 0;
}

// ==== ORIG 085defcc ====
#include "CDeathMatchBattleMgr/getMaxKillCount.inc"
{
    int max = 0;
    for (int i = 0; i <= 7; ++i)
    {
        if (m_killCount[i] > max)
            max = m_killCount[i];
    }
    return max;
}

// ==== ORIG 085def60 ====
// [b28d-27] NEAR N82附：ORIG 0x085def60；多实参 call 首值 callee-saved 寄存器对调
// （ORIG key→%ebx/idx→%esi，我方相反），语句表达式/具名形实测反劣 ⇒ 封顶。
// p167 CLEAR（3/3），语义闭合。
#include "CDeathMatchBattleMgr/InsertTimerKilledUserForRevive.inc"
{
    // ORIG 0x8568d90：key 先物化 %ebx、G_TimerQueue 次之 %esi（P24），无具名槽；
    // 源形=内联实参单语句（P134），禁中间局部（具名 key/queue 槽化致 0x38 帧）。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)5, roomIdx,
                                (TIMER_MESSAGE)(a + 0x33), 3,
                                m_pRoom->gen_timer_key((TIMER_MESSAGE)(a + 0x33)), 0);
}

// ==== ORIG 085df012 ====
// [L10b P292 档①] int→bool（头同步）：ORIG PvP_Room::check_winner 0x085d9950
// `test %al,%al; je` 无 setne。
#include "CDeathMatchBattleMgr/checkGameOverForSinglePlay.inc"
{
    if (mode == 0)
        return 1;
    // ORIG 跳转表 0x8cc1430：mode 2/3 同判 >1，4→>2，5→>3，6→>4，7→>5，8→>6。
    // case 按降序书写以对齐 ORIG 基本块布局（>6 块在最低地址）。
    switch (mode)
    {
    case 8:
        if (getMaxKillCount() > 6)
            return 1;
        return 0;
    case 7:
        if (getMaxKillCount() > 5)
            return 1;
        return 0;
    case 6:
        if (getMaxKillCount() > 4)
            return 1;
        return 0;
    case 5:
        if (getMaxKillCount() > 3)
            return 1;
        return 0;
    case 4:
        if (getMaxKillCount() > 2)
            return 1;
        return 0;
    case 3:
    case 2:
        if (getMaxKillCount() > 1)
            return 1;
        return 0;
    default:
        return 1;   // mode 0/1 与 >8 均判结束
    }
}

// ==== ORIG 085df126 ====
// SIGFIX b28c-L29：ORIG 串 "bool CDeathMatchBattleMgr::checkGameOverForTeamPlay(...)"。
// [b28d-27] DIFF N82：ORIG 0x085df126；p167 CLEAR（1/1）。
// [b28e-L6 三旋钮回填（探针 /tmp/l6p/p1.cpp 实证）]
//   ① 0x085df178 `je 085df1f7(nop)` + 0x085df1f5 `jmp 085df1f8` ⇒ guard 需负条件
//      goto 尾空标签（P223）；4.4 正包裹 `if(a!=0||b!=0)` 恒 jump-thread 掉 jmp+nop。
//   ② 0x085df213/216 与 0x085df21b/21e **两臂各自** store ⇒ 原码是 if/else 两臂赋值；
//      三目 `?:` 被 4.4 折成共享 store（少 1 条 mov）。
//   ③ case 叶块物理序=书写序（P324）：ORIG 块地址升序 0x085df23f(4)→0x253(3)→0x267(2)
//      →0x27b(1)→0x28f(default) ⇒ case 必须 4,3,2,1 降序书写（4.4 决策树不变、
//      `cmp $4; jne` fall-through 形态与 ORIG 一致）。
#include "CDeathMatchBattleMgr/checkGameOverForTeamPlay.inc"
    CUser** users, int teamCnt, unsigned char* teamWin)
{
    if (teamCnt == -1)
        return true;
    int killTeam1 = 0;
    int killTeam2 = 0;
    // [本波] ORIG 0x085df126 序言发三个零初始化 `movl $0x0,-0x18/-0x14/-0x10`
    // ⇒ maxKill 是**函数级具名 int（带 0 初值）**，三目赋值降为循环后的独立语句
    // （P28/P212 槽序：killTeam1 -0x18 → killTeam2 -0x14 → maxKill -0x10 → i -0xc）。
    int maxKill = 0;
    for (int i = 0; i <= 7; ++i)
    {
        // P223：负条件 goto 共享尾空标签复现 ORIG 的 nop 岛 + 分支尾 jmp。
        if (users[i] == 0 && m_killCount[i] == 0)
            goto nextUser;
        if (teamWin[i] == 1)
            killTeam1 += m_killCount[i];
        else if (teamWin[i] == 2)
            killTeam2 += m_killCount[i];
        else
            LogManager::logFormat(
                1, "pvp.cpp",
                __PRETTY_FUNCTION__,
                0x157f,
                "CDeathMatchBattleMgr::checkWinnerForTeamPlay , ERROR, bTeamState[%d] = %d",
                i, (unsigned int)teamWin[i]);
    nextUser: ;
    }
    if (killTeam1 > killTeam2)
        maxKill = killTeam1;
    else
        maxKill = killTeam2;
    // [本波·P56] ORIG 各 case 为 `cmpl $N,-0x10; jle <尾岛 mov $0>` + `mov $0x1,%eax`
    // 分支形（jle×4 + jmp×4）；直写 `return maxKill > N;` 被 4.4 折成 `setg` 单流 ⇒
    // 改 if-return 显式形。case 降序书写以对齐 ORIG 叶块物理序（见函数头③）。
    switch (teamCnt)
    {
    case 4:
        if (maxKill > 9)
            return true;
        return false;
    case 3:
        if (maxKill > 7)
            return true;
        return false;
    case 2:
        if (maxKill > 4)
            return true;
        return false;
    case 1:
        if (maxKill > 1)
            return true;
        return false;
    default:
        return true;
    }
}

// ==== ORIG 085df48a ====
#include "CDeathMatchBattleMgr/checkWinnerForSinglePlay.inc"
    CUser** users, bool* win, PvpUserTable& table)
{
    for (int i = 0; i <= 7; ++i)
    {
        if (users[i] == 0)
            m_killCount[i] = 0;
        win[i] = false;
    }
    int* pMax = std::max_element(m_killCount, m_killCount + 8);
    int winIdx = (int)(((unsigned int)pMax - (unsigned int)m_killCount) >> 2);
    if (winIdx <= 7)
        win[winIdx] = true;
    table.SetWinTeam(winIdx);
}

// ==== ORIG 085df296 ====
// [b28d-27] DIFF N82/N93：ORIG 0x085df296；p167 CLEAR（4/4）。
// [b28e-L6 全形回填（探针 /tmp/l6p/p5.cpp 逐条同构实证，帧 0x48/槽集全等）]
//   ① 两个循环共用**函数级 `int i`**（ORIG 序言只发一次 `movl $0x0,-0x14`，
//      第三循环亦复用 -0x14）⇒ 禁 for-init 内声明（4.4 会各开独立槽）。
//   ② 声明序 k1,k2,alive[2],i,t,winTeam ⇒ 4.4 槽 k1 -0x1c/k2 -0x18/
//      alive -0x24,-0x20/t -0x10/i -0x14/winTeam -0x9，与 ORIG 全等（P28/P331）。
//   ③ 第一循环 guard 负条件 goto 尾空标签（ORIG 0x085df2c3 `je 085df342(nop)`、
//      0x085df1f5/1f7 jmp+nop 岛）——4.4 正包裹恒 jump-thread。
//   ④ 第二循环 = 正包裹 `if (win[i] != 0)` + 内层 goto 岛串：ORIG 四出口为
//      0x085df374 `je 3c5`（直落 addl）/ 0x085df38e `je 3be(nop;jmp)` /
//      0x085df3a6 `js 3c1(nop;jmp)` / 0x085df3ac `jg 3c4(nop)`；4.4 只有此形给出
//      「1 直落 + 3 岛」组合（合并 &&/单 goto 形恒 0 岛或 4 岛）。
//   ⑤ `int t` 必须函数级（ORIG 0x085df364 `movl $0x0,-0x10` 与 ⑥ 同批发于循环后，
//      故写 `t = 0;` 赋值语句而非声明初值——声明初值会把该 store 提到函数头）。
#include "CDeathMatchBattleMgr/checkWinnerForTeamPlay.inc"
    CUser** users, bool* win, unsigned char* teamWin, PvpUserTable& table)
{
    int killTeam1 = 0;
    int killTeam2 = 0;
    // [本波·R20 建议] ORIG 0x085df296 用 `int winCount[2]`（基址 -0x24 + team 索引）
    // 而非两个独立标量 ⇒ 改数组形（索引访问形态与 ORIG 的 `-0x24(,%edx,4)` 一致）。
    // ⑥ 零初始化**位置**也是 ORIG 形状的一部分：0x085df356/35d/364 三条
    //    `movl $0x0,-0x24/-0x20/-0x10` 在**第一循环之后**才发射（函数头只有
    //    k1/k2/i 三条）⇒ 声明置函数头（保槽位 -0x24/-0x20/-0x10）而把初始化
    //    写成循环后的赋值语句（探针 /tmp/l6p/p7.cpp v_a 逐条同构）。
    int aliveTeam[2];
    int i;
    int t;
    unsigned char winTeam;
    for (i = 0; i <= 7; ++i)
    {
        // P223：负条件 goto 尾空标签复现 ORIG 的 nop 岛 + 分支尾 jmp。
        if (users[i] == 0)
            goto nextUser;
        if (teamWin[i] == 1)
            killTeam1 += m_killCount[i];
        else if (teamWin[i] == 2)
            killTeam2 += m_killCount[i];
        else
            LogManager::logFormat(
                1, "pvp.cpp",
                __PRETTY_FUNCTION__,
                0x1617,
                "CDeathMatchBattleMgr::checkWinnerForTeamPlay , ERROR, bTeamState[%d] = %d",
                i, (unsigned int)teamWin[i]);
    nextUser: ;
    }
    aliveTeam[0] = 0;
    aliveTeam[1] = 0;
    t = 0;
    for (i = 0; i <= 7; ++i)
    {
        if (win[i] != 0)
        {
            if (users[i] == 0)
                goto nextAlive;
            t = teamWin[i] - 1;
            if (t < 0)
                goto nextRange;
            if (t > 1)
                goto nextGrow;
            ++aliveTeam[t];
        nextGrow: ;
        nextRange: ;
        nextAlive: ;
        }
    }
    if (killTeam1 < killTeam2)
        winTeam = 2;
    else if (killTeam1 > killTeam2)
        winTeam = 1;
    else if (aliveTeam[0] > aliveTeam[1])
        winTeam = 1;
    else if (aliveTeam[1] > aliveTeam[0])
        winTeam = 2;
    else
        winTeam = (rand() & 1) ? 2 : 1;
    memset(win, 0, 8);
    for (i = 0; i <= 7; ++i)
    {
        if (teamWin[i] == winTeam)
            win[i] = true;
    }
    table.SetWinTeam((int)winTeam);
}

// ==== ORIG 085df576（const） ====
// [b28d-27] DIFF N82/N86：ORIG 0x085df576；p167 CLEAR（10/10），残差为 pair 数组
// 临时槽与帧分配，语义闭合。
#include "CDeathMatchBattleMgr/getRanking.inc"
{
    int count = 0;
    std::pair<int, int> rank[8];
    memset(out, 0, 0x20);
    for (int i = 0; i <= 7; ++i)
    {
        if (users[i] != 0)
        {
            rank[i] = std::make_pair(i, m_killCount[i]);
            ++count;
        }
        else
        {
            rank[i] = std::make_pair(i, -1);
        }
    }
    std::sort(rank, rank + 8, funDeathMatchKillCntCompare);
    for (int j = 0; j < count; ++j)
    {
        out[j] = rank[j].first;
        // [本波·P227] 全表达式临时链（Main 配方②）：ORIG 0x085df576 在 cMyTrace
        // ctor **之前**右起求 varargs 并以 callee-saved 跨 ctor 保活；具名两语句形
        // 使 4.4 只在 ctor 后求值（帧/槽差 0x10 的来源）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1695, 0)(
            "CDeathMatchBattleMgr::getRanking => Room(%d), Idx(%d), Rank(%d)",
            m_pRoom->get_index(), j, out[j]);
    }
    return count;
}

// ==== ORIG 085dee78 ====
#include "CDeathMatchBattleMgr/OnJoinRoom.inc"
{
    if (roomIdx < 0 || roomIdx > 7)
        return;
}

// ==== ORIG 085dee8a ====
#include "CDeathMatchBattleMgr/OnLeaveRoom.inc"
{
    if (roomIdx < 0 || roomIdx > 7)
        return;
}

// ==== ORIG 085deeae ====
#include "CDeathMatchBattleMgr/OnReady.inc"
{
    if (roomIdx < 0 || roomIdx > 7)
        return;
}

// ==== ORIG 085dee9c ====
#include "CDeathMatchBattleMgr/OnChangeTeam.inc"
{
    if (team < 0 || team > 7)
        return;
}

// ==== ORIG 085defc6 ====
#include "CDeathMatchBattleMgr/OnFight.inc"
{
}
