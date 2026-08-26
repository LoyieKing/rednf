// df_game_r Game/ CBloodDungeonRewardSecondEvent（血之副本结算金币 repeat 事件，event id 0x2e）。
// ORIG：派生自 CEventBase；+0x0c 存当前结算金币（ctor 置 0，StartEvent(Word_Param)
// 以零扩展 ushort 写入）。C1/C2 0x0811bf1e / D1/D2 0x0811bf44 / D0 0x0811bf74 /
// StartEvent() 0x0811be52 / StartEvent(Word_Param) 0x0811bea4 / EndEvent 0x0811becc /
// GetGold W 0x085bff26（return *(int*)(this+0xc)，逐指令转写）。
// 唯一调用方 CParty::make_blood_dungeon_info（报告 make_blood_dungeon_info.md：
// GetRepeatEvent(0x2e)->IsEventing 命中后静态调用 GetGold，优先于 FirstEvent/Jackpot）。
#ifndef DNF_CBLOODDUNGEONREWARDSECONDEVENT_H
#define DNF_CBLOODDUNGEONREWARDSECONDEVENT_H

#include "CGuildServerProxy.h"   // CEventBase / Word_Param
#include "GameTypes.h"           // cMyTrace

class CBloodDungeonRewardSecondEvent : public CEventBase
{
public:
    CBloodDungeonRewardSecondEvent();              // C1/C2 0x0811bf1e
    virtual ~CBloodDungeonRewardSecondEvent();     // D1/D2 0x0811bf44 / D0 0x0811bf74
    virtual void StartEvent();                     // 0x0811be52
    virtual void StartEvent(Word_Param param);     // 0x0811bea4（写 +0x0c 后虚调 StartEvent()）
    virtual void EndEvent();                       // 0x0811becc
    int GetGold();                                 // W 0x085bff26：return +0x0c

private:
    int m_gold;                                    // +0x0c（CEventBase=0x0c）
};

#endif  // DNF_CBLOODDUNGEONREWARDSECONDEVENT_H
