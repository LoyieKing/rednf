// EventGiveMeBox —— 台湾"GiveMeBox" 事件（CEventManager 叶事件）。
// ORIG vtable 0x08b7da40（实测 0x40）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 虚覆盖，4 = StartEvent(Word_Param)，其余槽继承 CEventBase。强符号（T）定义
// 于独立 TU（本 cpp）；C1 0x081649b0 / D1 0x081649cc / D0 0x081649fc /
// StartEvent() 0x08164a1a / StartEvent(Word_Param) 0x08164a6c / EndEvent 0x08164abe。
// kRewardItem_（ORIG R 0x08b7cd20，4 字节只读静态，值 0x2920918c）。
// 布局：CEventBase 基区（sizeof 0x0c），无扩展成员（C1 仅基类构造 + vptr）。
#ifndef DNF_GAME_EVENTGIVEMEBOX_H_
#define DNF_GAME_EVENTGIVEMEBOX_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class EventGiveMeBox : public CEventBase
{
public:
    EventGiveMeBox();
    virtual ~EventGiveMeBox();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);

    static const int kRewardItem_;   // ORIG R 0x08b7cd20
};

#endif // DNF_GAME_EVENTGIVEMEBOX_H_
