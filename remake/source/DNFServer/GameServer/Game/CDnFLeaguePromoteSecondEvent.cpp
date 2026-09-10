// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDnFLeaguePromoteSecondEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r DnF 联赛晋级第二事件 CDnFLeaguePromoteSecondEvent 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写，地址见头文件注释）。
#include "CDnFLeaguePromoteSecondEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811c1e2（弱 W）：仅基类构造 + vptr 置位。
#include "CDnFLeaguePromoteSecondEvent/CDnFLeaguePromoteSecondEvent.inc"
{
}

// ORIG 0x0811c1fe / 0x0811c22e（弱 W）：标准虚析构（空体编译器生成同形）。
#include "CDnFLeaguePromoteSecondEvent/dtor_CDnFLeaguePromoteSecondEvent.inc"
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c24c：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x2b,0)
// ("CDnFLeaguePromoteSecondEvent Event Start!")。
#include "CDnFLeaguePromoteSecondEvent/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x2b, 0);
    tr("CDnFLeaguePromoteSecondEvent Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c29e：thunk——经 vtable slot 2（StartEvent()）虚调用。
#include "CDnFLeaguePromoteSecondEvent/StartEvent_1.inc"
{
    // [N7 封顶][b28c-33 复核 12/12] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c2b8：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x35,0)
// ("CDnFLeaguePromoteSecondEvent Event End!")。
#include "CDnFLeaguePromoteSecondEvent/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x35, 0);
    tr("CDnFLeaguePromoteSecondEvent Event End!");
}
