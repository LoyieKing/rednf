// AttendanceEvent —— 签到事件（CEventBase 叶事件）。
// ORIG vtable 0x08b25ba0（实测）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 纯虚覆盖，4 = StartEvent(Word_Param)（完整实现，非 thunk；含 AddInfo 校验
// 与日志），其余槽继承 CEventBase。
// 布局：CEventBase 基区（vptr +0x00 / stADDINFO +0x04 / m_isEventing +0x08，
// sizeof 0x0c），无扩展成员（C1 0x080c828c 仅基类构造+vptr 置位，报告实证）。
// StartEvent(Word_Param) 将 param 拆两个 ushort 写入 m_addInfo
// （+0x04/+0x06），m_addInfo.m_info0 < 1 时 logFormat 报错，否则虚调
// StartEvent() 后打 trace（ORIG 0x080c8312 汇编，报告实证）。
// 报告：docs/class_func_reports/AttendanceEvent/ 全部 7 个方法已读。
#ifndef DNF_GAME_ATTENDANCEEVENT_H_
#define DNF_GAME_ATTENDANCEEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / stADDINFO / Word_Param

class AttendanceEvent : public CEventBase
{
public:
    AttendanceEvent();
    virtual ~AttendanceEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);

    int getRewordItem();    // ORIG T 0x080c8446：返回 (short)GetAddInfo()
};

#endif // DNF_GAME_ATTENDANCEEVENT_H_
