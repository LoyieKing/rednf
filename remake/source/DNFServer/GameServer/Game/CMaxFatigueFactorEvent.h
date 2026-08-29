// CMaxFatigueFactorEvent —— 最大疲劳系数事件（CEventBase 叶事件）。
// ORIG vtable 0x08bb7408（报告实证）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 覆盖，4 = StartEvent(Word_Param)，其余槽继承 CEventBase。
// 布局（ORIG ctor 0x081b2718 + StartEvent 0x081b278c 汇编实证）：
//   CEventBase 基区（vptr +0x00 / stADDINFO +0x04 / m_isEventing +0x08，
//   sizeof 0x0c）；int m_maxFatigueFactor @ +0x0c（C1/StartEvent/EndEvent 均
//   以 movl 写 0x64=100，StartEvent(Word_Param) 写 param.m_duration）。
#ifndef DNF_GAME_CMAXFATIGUEFACTOREVENT_H_
#define DNF_GAME_CMAXFATIGUEFACTOREVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CMaxFatigueFactorEvent : public CEventBase
{
public:
    CMaxFatigueFactorEvent();
    virtual ~CMaxFatigueFactorEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);

    int GetMaxFatigueFactor() { return m_maxFatigueFactor; }  // ORIG W 0x0812ce54
                                                              // （类内内联→O0 弱符号，与 ORIG call 同形）

    int m_maxFatigueFactor;   // +0x0c
};

#endif // DNF_GAME_CMAXFATIGUEFACTOREVENT_H_
