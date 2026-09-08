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
// b28c-L20: N7 — 唯一残差为虚调 StartEvent() 的间接 call 目标寄存器：ORIG 0x0811706e
// `mov (%eax),%edx; mov this,%eax; mov %eax,(%esp); call *%edx`，我们
// `mov (%eax),%eax; mov this,%edx; mov %edx,(%esp); call *%eax`。4.4 间接 call 目标恒
// %eax（probe4b 七形：裸调用 / this-> / (*this). / 基类指针 / 具名指针局部 / 返回值 /
// 局部中转 全部产 eax 目标）⇒ N7 封顶，ndiff=8 仅寄存器互换。
// 4.4.6 后端实测（/tmp/tu_CDeathTowerWinPointEvent_446.o，cc1plus446 驱动）：本件 ndiff=0
// ——4.4.6 间接 call 目标用 %edx、this 用 %eax（与 ORIG 4.1.2 同形）⇒ 路由切 4.4.6 后可 AE。
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
// b28c-L20: N7 — 残差仅虚调 IsEventing 的间接 call 目标寄存器互换（ORIG 目标 %edx /
// this %eax，我们目标 %eax / this %edx）；flds 100.0f 常量地址按 AE 归一已不计差。
// 4.4 间接 call 恒 %eax（probe4b 七形）⇒ N7 封顶，ndiff=8 仅寄存器互换。
// 4.4.6 后端实测（/tmp/tu_CDeathTowerWinPointEvent_446.o）：本件 ndiff=0 ⇒ 路由切 4.4.6 后可 AE。
float CDeathTowerWinPointEvent::getBonusRate()
{
    if (IsEventing((CUser*)0))
        return (float)m_ratio / 100.0f;
    return 0.0f;
}
