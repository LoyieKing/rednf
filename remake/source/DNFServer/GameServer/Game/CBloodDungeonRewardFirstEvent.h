// df_game_r Game/ CBloodDungeonRewardFirstEvent（血之副本结算金币 repeat 事件，event id 0x2d）。
// ORIG：派生自 CEventBase；+0x0c 存当前结算金币（ctor 置 0，StartEvent(Word_Param)
// 以零扩展 ushort 写入）。C1/C2 0x0811bdde / D1/D2 0x0811be04 / D0 0x0811be34 /
// StartEvent() 0x0811bd12 / StartEvent(Word_Param) 0x0811bd64 / EndEvent 0x0811bd8c /
// GetGold W 0x085bff1a（return *(int*)(this+0xc)，逐指令转写）。
// 唯一调用方 CParty::make_blood_dungeon_info（报告 make_blood_dungeon_info.md：
// GetRepeatEvent(0x2e)->IsEventing 失败后取 GetRepeatEvent(0x2d) 静态调用 GetGold）。
#ifndef DNF_CBLOODDUNGEONREWARDFIRSTEVENT_H
#define DNF_CBLOODDUNGEONREWARDFIRSTEVENT_H

#include "CGuildServerProxy.h"   // CEventBase / Word_Param
#include "GameTypes.h"           // cMyTrace

class CBloodDungeonRewardFirstEvent : public CEventBase
{
public:
    CBloodDungeonRewardFirstEvent();               // C1/C2 0x0811bdde
    virtual ~CBloodDungeonRewardFirstEvent();      // D1/D2 0x0811be04 / D0 0x0811be34
    virtual void StartEvent();                     // 0x0811bd12
    virtual void StartEvent(Word_Param param);     // 0x0811bd64（写 +0x0c 后虚调 StartEvent()）
    virtual void EndEvent();                       // 0x0811bd8c
    int GetGold();                                 // W 0x085bff1a：return +0x0c

private:
    int m_gold;                                    // +0x0c（CEventBase=0x0c）
};

#endif  // DNF_CBLOODDUNGEONREWARDFIRSTEVENT_H
