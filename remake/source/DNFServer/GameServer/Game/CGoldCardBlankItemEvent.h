// CGoldCardBlankItemEvent —— 金卡空白物品事件（CEventBase 叶事件）。
// ORIG vtable 0x08b545a8（报告实证）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 覆盖，4 = StartEvent(Word_Param)，其余槽继承 CEventBase。
// 布局（ORIG ctor 0x0811ce24 汇编实证）：仅 CEventBase 基区（sizeof 0x0c），
// 无扩展成员。
#ifndef DNF_GAME_CGOLDCARDBLANKITEMEVENT_H_
#define DNF_GAME_CGOLDCARDBLANKITEMEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CGoldCardBlankItemEvent : public CEventBase
{
public:
    CGoldCardBlankItemEvent();
    virtual ~CGoldCardBlankItemEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CGOLDCARDBLANKITEMEVENT_H_
