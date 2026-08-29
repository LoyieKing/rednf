// df_game_r 袭击开关事件 CAssaultOnOffEvent 实现。
// 报告缺失（§3.2 兜底）：ORIG 全部 W 弱符号，nm 定址 + objdump 逐指令转写：
//   C1 0x0811bb80 / D1 0x0811bb9c / D0 0x0811bbcc / StartEvent 0x0811bac2 /
//   StartEvent(Word_Param) 0x0811bb14（thunk）/ EndEvent 0x0811bb2e；
//   vtable 0x08b52288；PRETTY_FUNCTION 与日志字符串 .rodata 实测。
#include "CAssaultOnOffEvent.h"

#include "GameTypes.h"   // cMyTrace

// ORIG 0x0811bb80：仅基类构造 + vptr 置位。
CAssaultOnOffEvent::CAssaultOnOffEvent()
{
}

// ORIG 0x0811bb9c / 0x0811bbcc：空体 + 虚析构由编译器生成同形。
CAssaultOnOffEvent::~CAssaultOnOffEvent()
{
}

// ORIG 0x0811bac2：SetEventFlag(true) + cMyTrace(PRETTY, 0xe, 0)
//   ("Assault On Off Event Start!")。
void CAssaultOnOffEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__PRETTY_FUNCTION__, 0xe, 0);
    trace("Assault On Off Event Start!");
}

// ORIG 0x0811bb14（thunk，0x1a）：经 vptr+8 虚调 StartEvent()。
void CAssaultOnOffEvent::StartEvent(Word_Param /*param*/)
{
    StartEvent();
}

// ORIG 0x0811bb2e：SetEventFlag(false) + cMyTrace(PRETTY, 0x18, 0)
//   ("Assault On Off Event End!")。
void CAssaultOnOffEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x18, 0);
    trace("Assault On Off Event End!");
}
