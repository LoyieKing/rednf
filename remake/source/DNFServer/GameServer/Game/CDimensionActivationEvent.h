// df_game_r Game/ CDimensionActivationEvent（维度激活活动，重复事件槽 0x5e）。
// ORIG：派生自 CEventBase，无新增数据成员（对象布局 = CEventBase，0xc 字节）。
// C1 0x080eea84 / D1 0x080eeaa0 / D0 0x080eead0 / StartEvent() 0x080eeaee /
// EndEvent() 0x080eeb64 / StartEvent(Word_Param) 0x080eeb4a（转调 StartEvent()）/
// set_dungeon_play_count 0x080eebc0 / reset_dungeon_play_count 0x080eec58 /
// checkFreeRevivalCondition(int) 0x080eeca4（非虚；vtable +0x34 槽仍为基类
// CEventBase::IsEventing，本类不覆写）。
#ifndef DNF_CDIMENSIONACTIVATIONEVENT_H
#define DNF_CDIMENSIONACTIVATIONEVENT_H

#include "CGuildServerProxy.h"   // CEventBase / Word_Param

class CDimensionActivationEvent : public CEventBase
{
public:
    CDimensionActivationEvent();                       // 0x080eea84
    virtual ~CDimensionActivationEvent();              // D1 0x080eeaa0 / D0 0x080eead0
    virtual void StartEvent();                         // 0x080eeaee
    virtual void EndEvent();                           // 0x080eeb64
    virtual void StartEvent(Word_Param param);         // 0x080eeb4a
    void set_dungeon_play_count();                     // 0x080eebc0（非虚）
    void reset_dungeon_play_count();                   // 0x080eec58（非虚）
    bool checkFreeRevivalCondition(int dungeonDiff);   // 0x080eeca4（非虚）
};

#endif  // DNF_CDIMENSIONACTIVATIONEVENT_H
