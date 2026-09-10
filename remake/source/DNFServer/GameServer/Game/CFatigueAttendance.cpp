// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CFatigueAttendance/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 疲劳签到事件 CFatigueAttendance 实现。
// 报告缺失（docs/class_func_reports/CFatigueAttendance 不存在），按 §3.2
// 兜底：nm 定址（C1 0x081173ea / D1 0x08117406 / D0 0x08117436 /
// StartEvent 0x08117454 / StartEvent(Word_Param) 0x08117470 /
// EndEvent 0x081174fe，全部 W 弱符号）+ objdump 区间逐指令转写；
// __PRETTY_FUNCTION__ 字符串与 fmt 字符串取自 .rodata（nm 符号
// CFatigueAttendance::StartEvent(Word_Param)::__PRETTY_FUNCTION__ @0x08b4f000、
// ::EndEvent()::__PRETTY_FUNCTION__ @0x08b4f040、fmt @0x08b4de30/0x08b4de5f）。
#include "CFatigueAttendance.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x081173ea：仅基类构造 + vptr 置位（无自有成员）。
#include "CFatigueAttendance/CFatigueAttendance.inc"
{
}

// ORIG 0x08117406 / 0x08117436：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CFatigueAttendance/dtor_CFatigueAttendance.inc"
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08117454：仅 SetEventFlag(true)（无 trace）。
#include "CFatigueAttendance/StartEvent.inc"
{
    SetEventFlag(true);
}

// ---------------------------------------------------------------------------
// ORIG 0x08117470：param 两 ushort 写入 m_addInfo（+0x04/+0x06）→
// 经 vptr slot2 虚调 StartEvent() →
// cMyTrace(__PRETTY_FUNCTION__,0x1c,0)
// ("CFatigueAttendance(AddInfo:%d,%d) Event Start!", m_info0, m_info1)
// （读取以 movswl 符号扩展，变参 %d 求值序 = ORIG esi/ebx）。
#include "CFatigueAttendance/StartEvent_1.inc"
{
    // [b28c-27] N7 封顶：唯一残差=虚调用目标寄存器（ORIG 用 %edx，4.4 恒 %eax）；
    // 其余为 .o 重定位伪差（字符串立即数 + call 目标，AE 口径归一）。
    m_addInfo.m_info0 = param.m_duration;
    m_addInfo.m_info1 = param.m_field2;
    this->StartEvent();
    cMyTrace(__PRETTY_FUNCTION__, 0x1c, 0)(
        "CFatigueAttendance(AddInfo:%d,%d) Event Start!",
        (short)m_addInfo.m_info0, (short)m_addInfo.m_info1);
}

// ---------------------------------------------------------------------------
// ORIG 0x081174fe：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__,0x22,0)
// ("CFatigueAttendance Event End!")。
#include "CFatigueAttendance/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x22, 0);
    tr("CFatigueAttendance Event End!");
}
