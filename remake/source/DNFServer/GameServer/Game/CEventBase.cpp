// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CEventBase/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CEventBase 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 声明以 CGuildServerProxy.h 为准。
// 依据 docs/class_func_reports/CEventBase.md + ORIG elf 反汇编：
//   ctor 0x0810ae2c（调用 stADDINFO ctor 0x0810af10，+0x08 flag 置 0）
//   ~ctor 0x0810ae52 / StartEvent(Word_Param) 0x0810af26（空）
//   StartAction 0x080c84c4（空）/ AppendInfo 0x080c84ca（空）
//   GetAddInfo 0x080c84d0（返回 +0x04 的 4 字节）
//   SetEventFlag 0x080c84e2（写 +0x08）/ IsEventing 0x080c84fa（读 +0x08）
//   changeCharacName/dailyresetData/deliveryStart/deliveryEnd/delivery 空。
#include "CGuildServerProxy.h"
#include <iostream>   // std::ios_base::Init __ioinit（ORIG GI 键名 _ZN10CEventBaseC2Ev 的前提，P338）

// [P327 GI 键名] ORIG 本 TU `_GLOBAL__I_` 键名 = _ZN10CEventBaseC2Ev ⇒ CEventBase
// ctor 定义须为本 TU 首个强符号（此前 stADDINFO::stADDINFO 定义在前 ⇒ 键名错为
// _GLOBAL__I__ZN9stADDINFOC2Ev）。纯前置函数定义，符号集合/构造序零变化。
// [b28d-30] 复核 NEAR：ORIG 帧 0x18 vs 4.4 0x14（P190 附注：带 call 帧 sub 恒 16k+4
// 量化，0x18 非量化值不可凑；dead int×4=0x14、+double 跳 0x24）⇒ N82/P190 封顶。
#include "CEventBase/CEventBase.inc"
    : m_addInfo()
{
    // ORIG：*(vptr) = &vtable_08b4bae0（编译器自动）；
    // stADDINFO ctor 0x0810af10：两个 ushort 置 0xffff；+0x08 flag 置 0。
    // [N5F 2026-09-11 修·帧 0x28→0x18] ORIG 帧 0x18 = P210 三分律③：stADDINFO::C1
    // 定义虽已在本 TU（CEventBase.inc 之后），但位于本 caller 之后 ⇒ 4.4 看不到体、
    // 按保守 0x18 outgoing 分配。旧 dead int×4（P31 垫帧）在此之上再撑 0x10 ⇒ 0x28。
    // 删之即回归 0x18；探针 /tmp/r5/n5/N5F_EventFair/probe_frame.cpp 实测：
    // 无 dead int = sub $0x18（13 条逐条同形 / 与 ORIG 一致）、有 dead int = sub $0x28。
    m_isEventing = false;
}

// ORIG 0x0810af10：两个 ushort 均置 0xffff。
#include "CEventBase/stADDINFO.inc"
{
    m_info0 = 0xffff;
    m_info1 = 0xffff;
}

#include "CEventBase/dtor_CEventBase.inc"
{
    // ORIG 仅恢复基类 vptr；删除由 deleting destructor 负责。
}

#include "CEventBase/StartEvent.inc"
#include "CEventBase/StartAction.inc"
#include "CEventBase/AppendInfo.inc"

#include "CEventBase/GetAddInfo.inc"
{
    // ORIG 0x080c84d0：返回 this+0x04 的 stADDINFO（4 字节非 POD → gcc 4.4
    // sret：隐藏 out 指针 + this 栈参，ret $4；本定义编译产物与 ORIG 逐条同形，
    // 2026-08-28 OrigEventLeaf8f 由 int 修正为 stADDINFO，mangled 不变 Ev）。
    return m_addInfo;
}

#include "CEventBase/changeCharacName.inc"
#include "CEventBase/dailyresetData.inc"
#include "CEventBase/deliveryStart.inc"
#include "CEventBase/deliveryEnd.inc"
#include "CEventBase/delivery.inc"

#include "CEventBase/IsEventing.inc"
{
    // ORIG 0x080c84fa：movzbl 直返 +0x08 的 flag 字节。
    return m_isEventing;
}

#include "CEventBase/SetEventFlag.inc"
{
    // ORIG 0x080c84e2：写 +0x08 的 flag 字节。
    m_isEventing = flag;
}
