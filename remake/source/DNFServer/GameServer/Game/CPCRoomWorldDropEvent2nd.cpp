// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPCRoomWorldDropEvent2nd/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r PC 房世界掉落 2 期事件 CPCRoomWorldDropEvent2nd 实现。
// 报告缺失（全部 6 方法为 ORIG W 弱符号，objdump 转写）：
//   C1 0x0811bf92 / D1 0x0811bfae / D0 0x0811bfde /
//   StartEvent 0x0811bffc / StartEvent(Word_Param) 0x0811c04e / EndEvent 0x0811c068。
#include "CPCRoomWorldDropEvent2nd.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811bf92：仅基类构造 + vptr 置位。
#include "CPCRoomWorldDropEvent2nd/CPCRoomWorldDropEvent2nd.inc"
{
}

// ORIG 0x0811bfae / 0x0811bfde：空体 + 虚析构由编译器生成同形。
#include "CPCRoomWorldDropEvent2nd/dtor_CPCRoomWorldDropEvent2nd.inc"
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811bffc：置事件标志并打日志（__PRETTY_FUNCTION__，line 0x11）。
#include "CPCRoomWorldDropEvent2nd/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x11, 0);
    tr("PCRoom World Drop Event 2nd Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c04e：vtable+0x8 直调 StartEvent()（转发无参版本）。
// [b28c-31] N7 复核：转调虚调用 callee ORIG %edx vs 4.4 %eax（12=12）。
#include "CPCRoomWorldDropEvent2nd/StartEvent_1.inc"
{
    // [N7 封顶] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换，探针不可翻。
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c068：清事件标志并打日志（__PRETTY_FUNCTION__，line 0x1b）。
#include "CPCRoomWorldDropEvent2nd/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1b, 0);
    tr("PCRoom World Drop Event 2nd End!");
}
