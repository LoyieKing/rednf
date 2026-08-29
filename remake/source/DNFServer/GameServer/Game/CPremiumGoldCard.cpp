// df_game_r 黄金卡充值事件 CPremiumGoldCard 实现。
// 报告缺失（全部 6 方法为 ORIG W 弱符号，objdump 转写）：
//   C1 0x0811c432 / D1 0x0811c44e / D0 0x0811c47e /
//   StartEvent 0x0811c49c / StartEvent(Word_Param) 0x0811c4ee / EndEvent 0x0811c508。
#include "CPremiumGoldCard.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811c432：仅基类构造 + vptr 置位。
CPremiumGoldCard::CPremiumGoldCard()
{
}

// ORIG 0x0811c44e / 0x0811c47e：空体 + 虚析构由编译器生成同形。
CPremiumGoldCard::~CPremiumGoldCard()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c49c：置事件标志并打日志（__PRETTY_FUNCTION__，line 0x10）。
void CPremiumGoldCard::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x10, 0);
    tr("CPremiumGoldCard Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c4ee：vtable+0x8 直调 StartEvent()（转发无参版本）。
void CPremiumGoldCard::StartEvent(Word_Param param)
{
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c508：清事件标志并打日志（__PRETTY_FUNCTION__，line 0x1a）。
void CPremiumGoldCard::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1a, 0);
    tr("CPremiumGoldCard Event End!");
}
