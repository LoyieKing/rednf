// df_game_r Game/ CBurningFatigueEvent（批3：从 GameStubs.cpp 迁移）。
// ORIG：派生自 CEventBase；+0x0c 存 vector<stBurningFatigueParam_t>*（指向
// CDataManager+0x4f4）。C1 0x8109fa4 / D0 0x810a024 / D1 0x8109ff4 /
// StartEvent 0x810a042 / StartEvent(Word_Param) 0x810a048 / EndEvent 0x810a09a /
// getBonusExpRate 0x810a0ec / getRefillFatigue 0x810a166。
#ifndef DNF_CBURNINGFATIGUEEVENT_H
#define DNF_CBURNINGFATIGUEEVENT_H

#include <vector>

#include "CGuildServerProxy.h"   // CEventBase / Word_Param
#include "GameTypes.h"           // cMyTrace
#include "ServerParameterScript.h"   // stBurningFatigueParam_t（ORIG 权威布局）

class CExpDoubleEvent;  // 无关

class CBurningFatigueEvent : public CEventBase
{
public:
    CBurningFatigueEvent();                       // 0x08109fa4
    virtual ~CBurningFatigueEvent();              // D1 0x08109ff4 / D0 0x0810a024
    virtual void StartEvent();                    // 0x0810a042（空）
    virtual void StartEvent(Word_Param param);    // 0x0810a048
    virtual void EndEvent();                      // 0x0810a09a
    int getBonusExpRate(unsigned short level) const;    // 0x0810a0ec
    int getRefillFatigue(unsigned short level,
                         unsigned short cur) const;     // 0x0810a166

private:
    std::vector<stBurningFatigueParam_t>* m_params;   // +0x0c（G_CDataManager()+0x4f4）
    // 注意：m_params 指向 CDataManager 内 vector 本体（不是副本）。
};

#endif  // DNF_CBURNINGFATIGUEEVENT_H