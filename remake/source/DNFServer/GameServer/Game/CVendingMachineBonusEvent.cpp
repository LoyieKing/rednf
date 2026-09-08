// df_game_r 自动售货机 Bonus 事件 CVendingMachineBonusEvent 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写；地址见头文件注释）：
//   C1 0x08116dbe / D1 0x08116dda / D0 0x08116e0a /
//   StartEvent() 0x08116e28 / StartEvent(Word_Param) 0x08116e7a /
//   EndEvent 0x08116e94（均弱符号 W，独立 TU 定义）。
#include "CVendingMachineBonusEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x08116dbe：仅基类构造 + vptr 置位（无自有成员初始化）。
CVendingMachineBonusEvent::CVendingMachineBonusEvent()
{
}

// ORIG 0x08116dda / 0x08116e0a：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CVendingMachineBonusEvent::~CVendingMachineBonusEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08116e28：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x12,0)
// （ORIG 局部静态符号 0x08b4edc0 "virtual void CVendingMachineBonusEvent::StartEvent()"，
//  日志串 0x08b4dca8 "CVendingMachineBonusEvent Bonus Event Start!"）。
void CVendingMachineBonusEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x12, 0);
    tr("CVendingMachineBonusEvent Bonus Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08116e7a：thunk——经 vtable slot 2（StartEvent()）虚调用。
// [b28c-32] verdict=NEAR：N7 虚调用 callee 寄存器互换（12=12）。
void CVendingMachineBonusEvent::StartEvent(Word_Param param)
{
    // [N7 封顶] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x08116e94：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x1c,0)
// （ORIG 局部静态符号 0x08b4ed80 "virtual void CVendingMachineBonusEvent::EndEvent()"，
//  日志串 0x08b4dccc "CVendingMachineBonusEvent Bonus Event End!"）。
void CVendingMachineBonusEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1c, 0);
    tr("CVendingMachineBonusEvent Bonus Event End!");
}
