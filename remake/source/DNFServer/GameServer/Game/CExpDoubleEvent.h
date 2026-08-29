// df_game_r Game/ CExpDoubleEvent（批3：从 GameStubs.cpp 迁移）。
// ORIG：派生自 CEventBase；+0x0c 存当前 EXP 倍率（默认 100）。
// C1/C2 0x849ac44 / D0 0x849ac9a / D1/D2 0x849ac6a / StartEvent 0x849acb8 /
// StartEvent(Word_Param) 0x849ad14 / EndEvent 0x849ad72 / GetExpFactor 0x0814aad8 W。
#ifndef DNF_CEXPDOUBLEEVENT_H
#define DNF_CEXPDOUBLEEVENT_H

#include "CGuildServerProxy.h"   // CEventBase / Word_Param
#include "GameTypes.h"           // cMyTrace

class CExpDoubleEvent : public CEventBase
{
public:
    CExpDoubleEvent();                                   // 0x0849ac44
    virtual ~CExpDoubleEvent();                          // D1 0x0849ac6a / D0 0x0849ac9a
    virtual void StartEvent();                           // 0x0849acb8
    virtual void EndEvent();                             // 0x0849ad72
    virtual void StartEvent(Word_Param param);           // 0x0849ad14
    int GetExpFactor();                                  // 0x0814aad8：return +0x0c

private:
    int m_factor;                                        // +0x0c（CEventBase=0x0c）
};

#endif  // DNF_CEXPDOUBLEEVENT_H