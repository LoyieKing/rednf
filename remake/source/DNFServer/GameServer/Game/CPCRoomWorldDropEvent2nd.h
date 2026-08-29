// CPCRoomWorldDropEvent2nd —— PC 房世界掉落 2 期事件。
// 布局（ORIG ctor 0x0811bf92 实证）：仅 CEventBase 基区，无自有成员。
// 全部 6 方法为 ORIG W 弱符号（C1 0x0811bf92 / D1 0x0811bfae / D0 0x0811bfde /
// StartEvent 0x0811bffc / StartEvent(Word_Param) 0x0811c04e / EndEvent 0x0811c068，
// objdump 转写）。
#ifndef DNF_GAME_CPCROOMWORLDDROPEVENT2ND_H_
#define DNF_GAME_CPCROOMWORLDDROPEVENT2ND_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CPCRoomWorldDropEvent2nd : public CEventBase
{
public:
    CPCRoomWorldDropEvent2nd();
    virtual ~CPCRoomWorldDropEvent2nd();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CPCROOMWORLDDROPEVENT2ND_H_
