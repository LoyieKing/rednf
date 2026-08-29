// df_game_r Game/ CPCRoomFatigueEvent（网吧疲劳活动事件，重复事件槽）。
// ORIG：派生自 CEventBase（无额外成员）。
// C1 0x082675c0 / D1/D2 0x082675dc / D0 0x0826760c /
// StartEvent 0x0826762a / StartEvent(Word_Param) 0x0826767c /
// EndEvent 0x082676ce / IsEventing(CUser*) const 0x08267720。
#ifndef DNF_GAME_CPCROOMFATIGUEEVENT_H_
#define DNF_GAME_CPCROOMFATIGUEEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param
#include "GameTypes.h"           // cMyTrace

class CUser;

class CPCRoomFatigueEvent : public CEventBase
{
public:
    CPCRoomFatigueEvent();                       // 0x082675c0
    virtual ~CPCRoomFatigueEvent();              // D1 0x082675dc / D0 0x0826760c
    virtual void StartEvent(Word_Param param);   // 0x0826767c
    virtual void StartEvent();                   // 0x0826762a
    virtual void EndEvent();                     // 0x082676ce
    virtual bool IsEventing(CUser* user) const;  // 0x08267720（override vtable+0x3c）
};


#endif // DNF_GAME_CPCROOMFATIGUEEVENT_H_
