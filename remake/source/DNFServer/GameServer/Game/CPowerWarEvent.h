// CPowerWarEvent —— 力战事件（Power War，CEventManager event idx 0x1e）。
// ORIG vtable 0x08bf12e0（实测）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 纯虚覆盖，4 = StartEvent(Word_Param)，其余槽继承 CEventBase
// （StartAction/AppendInfo/GetAddInfo/changeCharacName/dailyresetData/
//  deliveryStart/deliveryEnd/delivery/IsEventing 与基类同序）。
// 布局（ORIG ctor 0x08267990 + StartEvent(Word_Param) 0x08267dea 汇编实证）：
//   CEventBase 基区（vptr +0x00 / stADDINFO +0x04 / m_isEventing +0x08，
//   sizeof 0x0c）；Word_Param m_param @ +0x0a（复用基类 tail padding：
//   StartEvent(Word_Param) 以整 4 字节 movl 写入，EndEvent 以 movw×2 清零）。
#ifndef DNF_GAME_CPOWERWAREVENT_H_
#define DNF_GAME_CPOWERWAREVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CPowerWarEvent : public CEventBase
{
public:
    CPowerWarEvent();
    virtual ~CPowerWarEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
    void CleanupPowerWarServerMessage();

    Word_Param m_param;   // +0x0a
};

#endif // DNF_GAME_CPOWERWAREVENT_H_
