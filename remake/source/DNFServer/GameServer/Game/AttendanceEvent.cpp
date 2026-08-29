// df_game_r 签到事件 AttendanceEvent 实现。
// 报告：docs/class_func_reports/AttendanceEvent/ 全部 7 个方法已读。
//   C1 0x080c828c / D1 0x080c82a8 / D0 0x080c82d8 / StartEvent 0x080c82f6 /
//   StartEvent(Word_Param) 0x080c8312 / EndEvent 0x080c83f4 /
//   getRewordItem 0x080c8446。
#include "AttendanceEvent.h"

#include "GameTypes.h"     // cMyTrace
#include "LogManager.h"    // LogManager::logFormat

// ORIG 0x080c828c：仅基类构造 + vptr 置位。
AttendanceEvent::AttendanceEvent()
{
}

// ORIG 0x080c82a8 / 0x080c82d8：空体 + 虚析构由编译器生成同形。
AttendanceEvent::~AttendanceEvent()
{
}

// ORIG 0x080c82f6：仅 SetEventFlag(true)，无 trace。
void AttendanceEvent::StartEvent()
{
    SetEventFlag(true);
}

// ORIG 0x080c8312：param 低 ushort → m_addInfo.m_info0(+0x04)、高 ushort →
// m_addInfo.m_info1(+0x06)；(short)m_info0 < 1 → logFormat 错误日志；否则
// 虚调 StartEvent() 后 cMyTrace(PRETTY, 0x1d, 0)("Attendance(AddInfo:%d,%d)
// Event Start!", (short)m_info0, (short)m_info1)。
void AttendanceEvent::StartEvent(Word_Param param)
{
    m_addInfo.m_info0 = param.m_duration;
    m_addInfo.m_info1 = param.m_field2;
    if ((short)m_addInfo.m_info0 < 1)
    {
        LogManager::logFormat(
            1, "AttendanceEvent.cpp",
            __PRETTY_FUNCTION__, 0x19,
            "Attendance(AddInfo:%d,%d) Event Start Error!",
            (int)(short)m_addInfo.m_info0, (int)(short)m_addInfo.m_info1);
    }
    else
    {
        StartEvent();
        // P37 链式临时：实参提前物化 esi/ebx 跨 cMyTrace 构造存活（ORIG 同形）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1d, 0)(
            "Attendance(AddInfo:%d,%d) Event Start!",
            (int)(short)m_addInfo.m_info0, (int)(short)m_addInfo.m_info1);
    }
}

// ORIG 0x080c83f4：SetEventFlag(false) + cMyTrace(PRETTY, 0x23, 0)
//   ("Attendacne Event End!")（ORIG 原文拼写 Attendacne）。
void AttendanceEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x23, 0);
    trace("Attendacne Event End!");
}

// ORIG 0x080c8446：直接 call CEventBase::GetAddInfo()（非虚，sret 写 out
// 指针；GetAddInfo 返回 stADDINFO，2026-08-28 OrigEventLeaf8f 修正 ABI），
// 返回 m_info0 符号扩展（movzwl+cwtl，与 ORIG 逐条同形；AE 实证）。
int AttendanceEvent::getRewordItem()
{
    return (short)CEventBase::GetAddInfo().m_info0;
}
