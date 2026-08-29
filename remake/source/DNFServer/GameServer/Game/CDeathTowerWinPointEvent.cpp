// df_game_r CDeathTowerWinPointEvent 实现（OrigMiscEvt 批补齐）。
// 声明见 CDeathTowerWinPointEvent.h。报告缺失，依据 ORIG nm+objdump 转写：
//   C1/C2 0x081170e8 D1/D2 0x0811710e D0 0x0811713e
//   StartEvent() 0x0811700e StartEvent(Word_Param) 0x0811706e
//   EndEvent 0x08117096 getBonusRate W 0x08469dcc
// vtable 0x08b4f3a0 = 基类 14 槽；+0xc m_ratio(unsigned)；
// getBonusRate 经 vptr+0x3c IsEventing 判活 → (double)m_ratio/100.0f。
#include "CDeathTowerWinPointEvent.h"
#include "GameTypes.h"   // cMyTrace

// ORIG 0x0811700e：置标志；trace 行号 16；(%x%%) 打印 m_ratio。
// 带参 trace：m_ratio 先物化 ebx 跨 ctor 存活（P37 链式临时形态）。
void CDeathTowerWinPointEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace(__PRETTY_FUNCTION__, 16, 0)("EVENT_FORM : Death Tower Win Point Event Start! (%x%%)", m_ratio);
}
CDeathTowerWinPointEvent::CDeathTowerWinPointEvent()
{
    m_ratio = 0;
}
CDeathTowerWinPointEvent::~CDeathTowerWinPointEvent() {}
// 0x0811706e：m_ratio = param.m_duration 后虚调 StartEvent()。
void CDeathTowerWinPointEvent::StartEvent(Word_Param param)
{
    m_ratio = (unsigned int)param.m_duration;
    StartEvent();
}
// 0x08117096：清标志；trace 行号 34。
void CDeathTowerWinPointEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 34, 0);
    trace("Death Tower Win Point Event End!");
}

// ORIG C1 0x081170e8：基类构造 + vptr + m_ratio(+0xc) 置 0；
// D1 0x0811710e 用户空析构（编译器生成 vptr+基类析构），D0 编译器自动。
// ORIG W 0x08469dcc _ZN24CDeathTowerWinPointEvent12getBonusRateEv：
// IsEventing(0) 虚调判活（vptr+0x3c）；活 → (float)m_ratio/100.0f。
// fildll 零扩展 + flds 100.0f + fdivrp（探针 f14 同形）→ 返回 float。
float CDeathTowerWinPointEvent::getBonusRate()
{
    if (IsEventing((CUser*)0))
        return (float)m_ratio / 100.0f;
    return 0.0f;
}
