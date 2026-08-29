// CAssaultOnOffEvent —— 袭击开关事件（CEventBase 叶事件）。
// ORIG vtable 0x08b52280（实测）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 纯虚覆盖，4 = StartEvent(Word_Param)（本类为 thunk，经 vptr+8 虚调
// StartEvent()），其余槽继承 CEventBase。
// 布局：CEventBase 基区（vptr +0x00 / stADDINFO +0x04 / m_isEventing +0x08，
// sizeof 0x0c），无扩展成员（C1 0x0811bb80 仅基类构造+vptr 置位，汇编实证）。
// 报告缺失（§3.2 兜底）：nm 定址 + objdump 逐指令转写，全部 W 弱符号。
#ifndef DNF_GAME_CASSAULTONOFFEVENT_H_
#define DNF_GAME_CASSAULTONOFFEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CAssaultOnOffEvent : public CEventBase
{
public:
    CAssaultOnOffEvent();
    virtual ~CAssaultOnOffEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CASSAULTONOFFEVENT_H_
