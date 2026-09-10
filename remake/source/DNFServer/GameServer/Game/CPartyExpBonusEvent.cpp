// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPartyExpBonusEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r CPartyExpBonusEvent 实现（LinkMisc3 批补齐；repeat 事件 0x18）。
// 布局：CEventBase（0xc）+ m_field0c（+0x0c，经验加成千分数）。
#include "CPartyExpBonusEvent.h"
#include "GameTypes.h"   // cMyTrace

// ORIG C1/C2 0x08116d4a：基类构造 + vptr 装载 + m_field0c = 0。
#include "CPartyExpBonusEvent/CPartyExpBonusEvent.inc"
{
    m_field0c = 0;
}

// ORIG D1/D2 0x08116d70：恢复基类 vptr + 基类析构（编译器自动）。
#include "CPartyExpBonusEvent/dtor_CPartyExpBonusEvent.inc"
{
}

// ORIG T 0x08116c70：置事件标志后经 cMyTrace 输出
// "Party Exp Bonus Event Start! (%x%%)"（实参为 m_field0c 千分数）。
#include "CPartyExpBonusEvent/StartEvent.inc"
{
    SetEventFlag(true);
    // [b28c-L15 P227] ORIG 在 cMyTrace ctor 前读 m_field0c 入 %ebx 跨 ctor 保活
    //   ⇒ 全表达式临时链形（具名对象则 ctor 后才读）。
    cMyTrace(__PRETTY_FUNCTION__, 0x10, 0)(
        "Party Exp Bonus Event Start! (%x%%)", m_field0c);
}

// ORIG T 0x08116cd0：m_field0c = param.m_duration 后转调无参 StartEvent()
//（经 vptr+0x08 槽虚调用）。
#include "CPartyExpBonusEvent/StartEvent_1.inc"
{
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
    m_field0c = (unsigned int)param.m_duration;
    StartEvent();
}

// ORIG T 0x08116cf8：清事件标志并输出 "Party Exp Bonus Event End!"。
#include "CPartyExpBonusEvent/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1b, 0);
    trace("Party Exp Bonus Event End!");
}

// ORIG W 0x085bfe1a _ZN19CPartyExpBonusEvent12getBonusRateEib
// 事件进行中 且 memberCount>1 且 !isHelpAbuse → m_field0c/100.0f，否则 0.0f。
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
#include "CPartyExpBonusEvent/getBonusRate.inc"
{
    // P0：ORIG 虚调结果直 test，无 bool 槽（右值内联）
    if (IsEventing((CUser*)0) && memberCount > 1 && !isHelpAbuse)
        return (float)m_field0c / 100.0f;
    return 0.0f;
}
