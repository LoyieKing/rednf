// df_game_r Game/ CPCRoomBurningEvent（网吧燃烧活动，重复事件槽 8）。
// ORIG：派生自 CEventBase；+0x0c 存当前 EXP 倍率（默认 100）。
// C1 0x082671dc / D0 0x08267232 / D1/D2 0x08267202 / StartEvent 0x08267250 /
// StartEvent(Word_Param) 0x082672ac / EndEvent 0x0826730a /
// IsEventing(CUser*) const 0x08267366 / GetExpFactor 0x0814aae4 W。
#ifndef DNF_CPCROOMBURNINGEVENT_H
#define DNF_CPCROOMBURNINGEVENT_H

#include "CGuildServerProxy.h"   // CEventBase / Word_Param
#include "GameTypes.h"           // cMyTrace

class CUser;

class CPCRoomBurningEvent : public CEventBase
{
public:
    CPCRoomBurningEvent();                          // 0x082671dc
    virtual ~CPCRoomBurningEvent();                 // D1 0x08267202 / D0 0x08267232
    virtual void StartEvent();                      // 0x08267250
    virtual void StartEvent(Word_Param param);      // 0x082672ac
    virtual void EndEvent();                        // 0x0826730a
    virtual bool IsEventing(CUser* user) const;     // 0x08267366（override vtable+0x3c）
    int GetExpFactor();                             // 0x0814aae4：return +0x0c

private:
    int m_factor;                                   // +0x0c（CEventBase=0x0c）
};

#endif  // DNF_CPCROOMBURNINGEVENT_H
