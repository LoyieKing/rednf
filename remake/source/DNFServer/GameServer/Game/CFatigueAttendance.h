// CFatigueAttendance —— 疲劳签到事件（CEventBase 叶事件）。
// ORIG vtable 0x08b4f2e0（objdump 实证，报告缺失按 §3.2 兜底）：
// 0/1 虚析构，2/3 = StartEvent()/EndEvent() 覆盖，4 = StartEvent(Word_Param)
// （thunk：经 vptr slot2 虚调 StartEvent()），其余槽继承 CEventBase。
// 布局（ORIG ctor 0x081173ea + StartEvent(Word_Param) 0x08117470 汇编实证）：
//   仅 CEventBase 基区（sizeof 0x0c）；StartEvent(Word_Param) 将
//   param.m_duration/m_field2 以 16 位 mov 写入基类 m_addInfo.m_info0/+0x04
//   与 m_info1/+0x06（stADDINFO 两 ushort），读取处以 movswl 符号扩展
//   作 trace 变参。
#ifndef DNF_GAME_CFATIGUEATTENDANCE_H_
#define DNF_GAME_CFATIGUEATTENDANCE_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param / stADDINFO（ORIG 权威声明）

class CFatigueAttendance : public CEventBase
{
public:
    CFatigueAttendance();
    virtual ~CFatigueAttendance();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CFATIGUEATTENDANCE_H_
