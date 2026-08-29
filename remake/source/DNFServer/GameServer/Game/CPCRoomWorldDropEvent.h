// CPCRoomWorldDropEvent —— PC 房世界掉落事件。
// 布局（ORIG ctor 0x082677f8 实证）：仅 CEventBase 基区，无自有成员。
// StartEvent/StartEvent(Word_Param)/EndEvent 为 ORIG W 弱符号
// （0x082678be/0x08267910/0x0826792a，objdump 转写）。
#ifndef DNF_GAME_CPCROOMWORLDDROPEVENT_H_
#define DNF_GAME_CPCROOMWORLDDROPEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CPCRoomWorldDropEvent : public CEventBase
{
public:
    CPCRoomWorldDropEvent();
    virtual ~CPCRoomWorldDropEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CPCROOMWORLDDROPEVENT_H_
