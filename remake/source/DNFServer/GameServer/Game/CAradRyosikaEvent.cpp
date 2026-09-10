// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CAradRyosikaEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 阿拉德凉席（Ryosika）事件 CAradRyosikaEvent 实现 —— 本 TU 仅实现
// C1/D1/D0/StartEvent()/StartEvent(Word_Param)/EndEvent 六个方法；
// ApplyDatePeriod/SendMail/DoEvent 定义在 Inter_LoadEtc_impl.cpp（LinkClosureB-2）。
// 报告：docs/class_func_reports/CAradRyosikaEvent/ 全部 9 个方法已读。
//   C1 0x081a0ea0 / D1 0x081a0ed0 / D0 0x081a0f00 / StartEvent 0x081a0f1e /
//   StartEvent(Word_Param) 0x081a0f8a / EndEvent 0x081a0fbe。
// 布局（权威头）：CEventBase 基区 + int m_field0c@+0x0c + int m_field10@+0x10
//   （ctor 双双置 -1）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN17CAradRyosikaEventC2Ev`（ORIG 0x81a1139）；ctor 已是首强符号。
#include <iostream>

#include "CAradRyosikaEvent.h"

#include "GameTypes.h"   // cMyTrace

// ORIG 0x081a0ea0：基类构造 + vptr 置位 + m_field0c/m_field10 置 -1。
#include "CAradRyosikaEvent/CAradRyosikaEvent.inc"
{
    m_field0c = -1;
    m_field10 = -1;
}

// ORIG 0x081a0ed0 / 0x081a0f00：空体 + 虚析构由编译器生成同形。
#include "CAradRyosikaEvent/dtor_CAradRyosikaEvent.inc"
{
}

// ORIG 0x081a0f1e：cMyTrace(__FUNCTION__, 0x14, 0)("Arad Ryosika Event Start!
//   [step:%d][job:%d]", m_field0c, m_field10) 在 SetEventFlag(true) 之前
//   （trace 先、flag 后，ORIG 汇编序）。__FUNCTION__ 名（0x0b 长度符号：
//   ORIG r _ZZN17CAradRyosikaEvent10StartEventEvE12__FUNCTION__ @0x08bab950
//   sz 0x0b，"StartEvent\0"）。必须传 __FUNCTION__ 本体（P139），手写字面量
//   只生成匿名 .rodata，丢 ORIG 局部静态符号。
#include "CAradRyosikaEvent/StartEvent.inc"
{
    // P37 链式临时：实参提前物化 ebx/esi 跨 cMyTrace 构造存活，与 ORIG 同形。
    cMyTrace(__FUNCTION__, 0x14, 0)(
        "Arad Ryosika Event Start! [step:%d][job:%d]", m_field0c, m_field10);
    SetEventFlag(true);
}

// ORIG 0x081a0f8a：m_field0c = param.m_duration（零扩展 ushort），
//   m_field10 = param.m_field2，然后经 vptr+8 虚调 StartEvent()。
#include "CAradRyosikaEvent/StartEvent_1.inc"
{
    // [b28c-L18] N7 封顶复核（fresh .o 20=20）：唯一残差=虚调用 callee 寄存器互换
    // ——ORIG `mov (%eax),%edx; …; call *%edx` vs 4.4 恒间接 call 目标 %eax
    //（探针 /tmp/l22p/vprobe*.cpp + 本轮复查），勿再耗轮次。
    m_field0c = param.m_duration;
    m_field10 = param.m_field2;
    StartEvent();
}

// ORIG 0x081a0fbe：仅 SetEventFlag(false)，无 trace。
#include "CAradRyosikaEvent/EndEvent.inc"
{
    SetEventFlag(false);
}
