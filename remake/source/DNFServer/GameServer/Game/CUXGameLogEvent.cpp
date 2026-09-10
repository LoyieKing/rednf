// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUXGameLogEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r U/X Game Log 事件 CUXGameLogEvent 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写；地址见头文件注释）：
//   C1 0x08117678 / D1 0x08117694 / D0 0x081176c4 /
//   StartEvent() 0x081176e2 / StartEvent(Word_Param) 0x08117734 /
//   EndEvent 0x0811774e（均弱符号 W，独立 TU 定义）。
#include "CUXGameLogEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x08117678：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "CUXGameLogEvent/CUXGameLogEvent.inc"
{
}

// ORIG 0x08117694 / 0x081176c4：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CUXGameLogEvent/dtor_CUXGameLogEvent.inc"
{
}

// ---------------------------------------------------------------------------
// ORIG 0x081176e2：SetEventFlag(true) + cMyTrace(<prett>,0x13,0)
// ("CUXGameLogEvent Event Start!")。
// [Q7SemanticAxis 2026-09-11] §6.5 串内容轴：ORIG 第一实参是 __PRETTY_FUNCTION__ 展开
// `virtual void CUXGameLogEvent::StartEvent()`（实读 0x08b4f140 size 0x2b = 42 字符 + NUL；
// 我方曾手写 "CUXGameLogEvent::StartEvent()" ⇒ 缺 `virtual void ` 前缀，trace 文本与 ORIG 不符，
// 且 prett 符号不发射）。本函数为 virtual ⇒ 宏展开即带 `virtual ` 前缀，逐字节对齐。
#include "CUXGameLogEvent/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x13, 0);
    tr("CUXGameLogEvent Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08117734：thunk——经 vtable slot 2（StartEvent()）虚调用。
#include "CUXGameLogEvent/StartEvent_1.inc"
{
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x0811774e：SetEventFlag(false) + cMyTrace(<prett>,0x1e,0)
// ("CUXGameLogEvent Event End!")。
// [Q7SemanticAxis 2026-09-11] 同上：ORIG prett 串实读 0x08b4f100 size 0x29 =
// `virtual void CUXGameLogEvent::EndEvent()`（40 字符 + NUL）。
#include "CUXGameLogEvent/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1e, 0);
    tr("CUXGameLogEvent Event End!");
}
