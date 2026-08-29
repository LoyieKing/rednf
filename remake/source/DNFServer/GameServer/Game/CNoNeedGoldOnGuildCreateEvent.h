// CNoNeedGoldOnGuildCreateEvent —— 创建公会免金币事件。
// 布局（ORIG ctor 0x081b9b8c 实证）：仅 CEventBase 基区，无自有成员。
#ifndef DNF_GAME_CNONEEDGOLDONGUILDCREATEEVENT_H_
#define DNF_GAME_CNONEEDGOLDONGUILDCREATEEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CNoNeedGoldOnGuildCreateEvent : public CEventBase
{
public:
    CNoNeedGoldOnGuildCreateEvent();
    virtual ~CNoNeedGoldOnGuildCreateEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CNONEEDGOLDONGUILDCREATEEVENT_H_
