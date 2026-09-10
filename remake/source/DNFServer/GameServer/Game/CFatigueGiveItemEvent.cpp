// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CFatigueGiveItemEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 疲劳赠送物品事件 CFatigueGiveItemEvent 实现。
// 报告缺失（docs/class_func_reports/CFatigueGiveItemEvent 不存在），按 §3.2
// 兜底：nm 定址（C1 0x0811c838 / D1 0x0811c854 / D0 0x0811c884 /
// StartEvent 0x0811c8a2 / StartEvent(Word_Param) 0x0811c8f4 /
// EndEvent 0x0811c90e，全部 W 弱符号）+ objdump 区间逐指令转写；
// __PRETTY_FUNCTION__ 字符串与 fmt 字符串取自 .rodata（nm 符号
// CFatigueGiveItemEvent::StartEvent()::__PRETTY_FUNCTION__ @0x08b51f20、
// ::EndEvent()::__PRETTY_FUNCTION__ @0x08b51ee0、fmt @0x08b4fcfc/0x08b4fd20）。
#include "CFatigueGiveItemEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811c838：仅基类构造 + vptr 置位（无自有成员）。
#include "CFatigueGiveItemEvent/CFatigueGiveItemEvent.inc"
{
}

// ORIG 0x0811c854 / 0x0811c884：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CFatigueGiveItemEvent/dtor_CFatigueGiveItemEvent.inc"
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c8a2：SetEventFlag(true) +
// cMyTrace(__PRETTY_FUNCTION__@0x08b51f20,0x12,0)
// ("CFatigueGiveItemEvent Event Start!"@0x08b4fcfc)。
#include "CFatigueGiveItemEvent/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x12, 0);
    tr("CFatigueGiveItemEvent Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c8f4：thunk —— 经 vptr slot2 虚调 StartEvent()（忽略 param）。
#include "CFatigueGiveItemEvent/StartEvent_1.inc"
{
    this->StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c90e：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__@0x08b51ee0,0x1c,0)
// ("CFatigueGiveItemEvent Event End!"@0x08b4fd20)。
#include "CFatigueGiveItemEvent/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1c, 0);
    tr("CFatigueGiveItemEvent Event End!");
}
