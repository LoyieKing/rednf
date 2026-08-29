// CNotApplyBalkeunEvent —— 不适用巴尔肯（Balkeun）事件。
// 布局（ORIG ctor 0x081b9d5c 实证）：仅 CEventBase 基区，无自有成员。
#ifndef DNF_GAME_CNOTAPPLYBALKEUNEVENT_H_
#define DNF_GAME_CNOTAPPLYBALKEUNEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CNotApplyBalkeunEvent : public CEventBase
{
public:
    CNotApplyBalkeunEvent();
    virtual ~CNotApplyBalkeunEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CNOTAPPLYBALKEUNEVENT_H_
