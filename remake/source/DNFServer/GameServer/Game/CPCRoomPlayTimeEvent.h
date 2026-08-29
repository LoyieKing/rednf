// df_game_r Game/ CPCRoomPlayTimeEvent（网吧时长活动事件，重复事件槽）。
// ORIG：派生自 CEventBase（无额外成员）。
// C1 0x0811c55a / D1/D2 0x0811c576 / D0 0x0811c5a6 /
// StartEvent 0x0811c5c4 / StartEvent(Word_Param) 0x0811c616（虚转发 StartEvent()）/
// EndEvent 0x0811c630（清网吧时长数据）/ StartAction 0x0811c754。
#ifndef DNF_GAME_CPCROOMPLAYTIMEEVENT_H_
#define DNF_GAME_CPCROOMPLAYTIMEEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param
#include "GameTypes.h"           // cMyTrace

class CPCRoomPlayTimeEvent : public CEventBase
{
public:
    CPCRoomPlayTimeEvent();                      // 0x0811c55a
    virtual ~CPCRoomPlayTimeEvent();             // D1 0x0811c576 / D0 0x0811c5a6
    virtual void StartEvent(Word_Param param);   // 0x0811c616
    virtual void StartEvent();                   // 0x0811c5c4
    virtual void EndEvent();                     // 0x0811c630
    virtual void StartAction();                  // 0x0811c754（override vtable+0x1c）
};


#endif // DNF_GAME_CPCROOMPLAYTIMEEVENT_H_
