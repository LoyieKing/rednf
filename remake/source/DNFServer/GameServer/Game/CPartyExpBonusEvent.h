// df_game_r CPartyExpBonusEvent —— repeat 事件 0x18（party exp bonus event）。
// ORIG 符号（nm，本批全部补齐定义于 CPartyExpBonusEvent.cpp）：
// C1/C2 0x08116d4a、D1/D2 0x08116d70、StartEvent 0x08116c70、
// StartEvent(Word_Param) 0x08116cd0、EndEvent 0x08116cf8、
// getBonusRate(int memberCount, bool isHelpAbuse) 0x085bfe1a（W）。
#ifndef DNF_CPARTYEXPBONUSEVENT_H_
#define DNF_CPARTYEXPBONUSEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase

class CPartyExpBonusEvent : public CEventBase
{
public:
    CPartyExpBonusEvent();
    virtual ~CPartyExpBonusEvent();
    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);

    float getBonusRate(int memberCount, bool isHelpAbuse);  // ORIG W 0x085bfe1a

private:
    // +0x0c：ctor 清零；StartEvent(Word_Param) 写入 param.m_duration
    //（经验加成千分数）；getBonusRate 以 /100.0f 消费。
    unsigned int m_field0c;
};

#endif  // DNF_CPARTYEXPBONUSEVENT_H_
