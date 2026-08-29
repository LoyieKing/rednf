// df_game_r 燃烧的金币怪物事件 CBurnigGoldMonsterEvent 实现。
// 报告缺失（§3.2 兜底）：ORIG 全部 W 弱符号，nm 定址 + objdump 逐指令转写：
//   C1 0x08116ee6 / D1 0x08116f02 / D0 0x08116f32 / StartEvent 0x08116f50 /
//   StartEvent(Word_Param) 0x08116fa2（thunk）/ EndEvent 0x08116fbc；
//   vtable 0x08b4f3e0；PRETTY_FUNCTION 与日志字符串 .rodata 实测。
#include "CBurnigGoldMonsterEvent.h"

#include "GameTypes.h"   // cMyTrace

// ORIG 0x08116ee6：仅基类构造 + vptr 置位（无自有成员初始化）。
CBurnigGoldMonsterEvent::CBurnigGoldMonsterEvent()
{
}

// ORIG 0x08116f02 / 0x08116f32：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CBurnigGoldMonsterEvent::~CBurnigGoldMonsterEvent()
{
}

// ORIG 0x08116f50：SetEventFlag(true) + cMyTrace(PRETTY, 0x12, 0)
//   ("CBurnigGoldMonsterEvent Event Start!")。
void CBurnigGoldMonsterEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x12, 0);
    trace("CBurnigGoldMonsterEvent Event Start!");
}

// ORIG 0x08116fa2（thunk，0x1a）：经 vptr+8 虚调 StartEvent()。
void CBurnigGoldMonsterEvent::StartEvent(Word_Param /*param*/)
{
    StartEvent();
}

// ORIG 0x08116fbc：SetEventFlag(false) + cMyTrace(PRETTY, 0x1c, 0)
//   ("CBurnigGoldMonsterEvent Event End!")。
void CBurnigGoldMonsterEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1c, 0);
    trace("CBurnigGoldMonsterEvent Event End!");
}
