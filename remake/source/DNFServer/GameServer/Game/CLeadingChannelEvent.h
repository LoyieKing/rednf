// CLeadingChannelEvent —— 引导频道事件（CEventBase 叶事件）。
// ORIG vtable 0x08b588a8（报告实证）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 覆盖，4 = StartEvent(Word_Param)，其余槽继承 CEventBase。
// 布局（ORIG ctor 0x0811fd64 汇编实证）：
//   CEventBase 基区（sizeof 0x0c）；int m_channelFactor @ +0x0c
//   （C1/StartEvent/EndEvent 均 movl 写 0x64=100，StartEvent(Word_Param)
//   写 param.m_duration）。
#ifndef DNF_GAME_CLEADINGCHANNELEVENT_H_
#define DNF_GAME_CLEADINGCHANNELEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CLeadingChannelEvent : public CEventBase
{
public:
    CLeadingChannelEvent();
    virtual ~CLeadingChannelEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);

    int m_channelFactor;   // +0x0c
};

#endif // DNF_GAME_CLEADINGCHANNELEVENT_H_
