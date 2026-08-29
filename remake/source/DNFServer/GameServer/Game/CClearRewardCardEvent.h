// CClearRewardCardEvent —— 清奖励卡事件（CEventBase 叶事件）。
// ORIG 报告：docs/class_func_reports/CClearRewardCardEvent/（6 函数全读）。
//   C1 0x0810a720 / D1 0x0810a73c / D0 0x0810a76c / StartEvent() 0x0810a78a（空体）/
//   StartEvent(Word_Param) 0x0810a790 / EndEvent 0x0810a7e2（强符号 T）。
// 布局：仅 CEventBase 基区（sizeof 0x0c），无自有成员（C1 仅基类构造 + vptr）。
#ifndef DNF_GAME_CCLEARREWARDCARDEVENT_H_
#define DNF_GAME_CCLEARREWARDCARDEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CClearRewardCardEvent : public CEventBase
{
public:
    CClearRewardCardEvent();
    virtual ~CClearRewardCardEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CCLEARREWARDCARDEVENT_H_
