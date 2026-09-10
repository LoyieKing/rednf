// df_game_r PC 房世界掉落事件 CPCRoomWorldDropEvent 实现。
// 报告：docs/class_func_reports/CPCRoomWorldDropEvent/（C1/D1/D0 有报告；
// StartEvent/StartEvent(Word_Param)/EndEvent 为 ORIG W 弱符号
// 0x082678be/0x08267910/0x0826792a，objdump 转写）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN21CPCRoomWorldDropEventC2Ev`（ORIG 0x82678a2）；ctor 已是首强符号。
#include <iostream>

#include "CPCRoomWorldDropEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x082677f8：仅基类构造 + vptr 置位。
CPCRoomWorldDropEvent::CPCRoomWorldDropEvent()
{
}

// ORIG 0x08267814 / 0x08267844：空体 + 虚析构由编译器生成同形。
CPCRoomWorldDropEvent::~CPCRoomWorldDropEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x082678be：置事件标志并打日志（__FUNCTION__ "StartEvent"，line 0x21）。
void CPCRoomWorldDropEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x21, 0);
    tr("PCRoom World Drop Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08267910：vtable+0x8 直调 StartEvent()（转发无参版本）。
// [b28c-30] N7 封顶：ORIG 取槽后 `mov (%eax),%edx; mov 0x8(%ebp),%eax;
// mov %eax,(%esp); call *%edx`（4.1 间接目标 %edx）；4.4 恒目标 %eax
// （`mov (%eax),%eax; …; call *%eax`）。助记符序列全等仅寄存器名差。
void CPCRoomWorldDropEvent::StartEvent(Word_Param param)
{
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x0826792a：清事件标志并打日志（__FUNCTION__ "EndEvent"，line 0x2c）。
void CPCRoomWorldDropEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x2c, 0);
    tr("PCRoom World Drop Event End!");
}
