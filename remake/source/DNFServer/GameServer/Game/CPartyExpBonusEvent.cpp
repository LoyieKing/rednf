// df_game_r CPartyExpBonusEvent 实现（LinkMisc3 批补齐；repeat 事件 0x18）。
// 布局：CEventBase（0xc）+ m_field0c（+0x0c，经验加成千分数）。
#include "CPartyExpBonusEvent.h"
#include "GameTypes.h"   // cMyTrace

// ORIG C1/C2 0x08116d4a：基类构造 + vptr 装载 + m_field0c = 0。
CPartyExpBonusEvent::CPartyExpBonusEvent()
{
    m_field0c = 0;
}

// ORIG D1/D2 0x08116d70：恢复基类 vptr + 基类析构（编译器自动）。
CPartyExpBonusEvent::~CPartyExpBonusEvent()
{
}

// ORIG T 0x08116c70：置事件标志后经 cMyTrace 输出
// "Party Exp Bonus Event Start! (%x%%)"（实参为 m_field0c 千分数）。
void CPartyExpBonusEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace("virtual void CPartyExpBonusEvent::StartEvent()", 0x10, 0);
    trace("Party Exp Bonus Event Start! (%x%%)", m_field0c);
}

// ORIG T 0x08116cd0：m_field0c = param.m_duration 后转调无参 StartEvent()
//（经 vptr+0x08 槽虚调用）。
void CPartyExpBonusEvent::StartEvent(Word_Param param)
{
    m_field0c = (unsigned int)param.m_duration;
    StartEvent();
}

// ORIG T 0x08116cf8：清事件标志并输出 "Party Exp Bonus Event End!"。
void CPartyExpBonusEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace("virtual void CPartyExpBonusEvent::EndEvent()", 0x1b, 0);
    trace("Party Exp Bonus Event End!");
}

// ORIG W 0x085bfe1a _ZN19CPartyExpBonusEvent12getBonusRateEib
// 事件进行中 且 memberCount>1 且 !isHelpAbuse → m_field0c/100.0f，否则 0.0f。
float CPartyExpBonusEvent::getBonusRate(int memberCount, bool isHelpAbuse)
{
    bool eventing = IsEventing((CUser*)0);
    if (eventing && memberCount > 1 && !isHelpAbuse)
        return (float)m_field0c / 100.0f;
    return 0.0f;
}
