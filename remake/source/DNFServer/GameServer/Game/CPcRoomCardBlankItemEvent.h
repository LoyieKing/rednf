// CPcRoomCardBlankItemEvent —— PC 房卡片空白道具事件。
// 布局（ORIG ctor 0x0826743c 实证）：仅 CEventBase 基区，无自有成员。
#ifndef DNF_GAME_CPCROOMCARDBLANKITEMEVENT_H_
#define DNF_GAME_CPCROOMCARDBLANKITEMEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CPcRoomCardBlankItemEvent : public CEventBase
{
public:
    CPcRoomCardBlankItemEvent();
    virtual ~CPcRoomCardBlankItemEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CPCROOMCARDBLANKITEMEVENT_H_
