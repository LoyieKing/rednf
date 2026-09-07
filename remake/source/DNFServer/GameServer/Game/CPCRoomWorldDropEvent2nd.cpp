// df_game_r PC 房世界掉落 2 期事件 CPCRoomWorldDropEvent2nd 实现。
// 报告缺失（全部 6 方法为 ORIG W 弱符号，objdump 转写）：
//   C1 0x0811bf92 / D1 0x0811bfae / D0 0x0811bfde /
//   StartEvent 0x0811bffc / StartEvent(Word_Param) 0x0811c04e / EndEvent 0x0811c068。
#include "CPCRoomWorldDropEvent2nd.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811bf92：仅基类构造 + vptr 置位。
CPCRoomWorldDropEvent2nd::CPCRoomWorldDropEvent2nd()
{
}

// ORIG 0x0811bfae / 0x0811bfde：空体 + 虚析构由编译器生成同形。
CPCRoomWorldDropEvent2nd::~CPCRoomWorldDropEvent2nd()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811bffc：置事件标志并打日志（__PRETTY_FUNCTION__，line 0x11）。
void CPCRoomWorldDropEvent2nd::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x11, 0);
    tr("PCRoom World Drop Event 2nd Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c04e：vtable+0x8 直调 StartEvent()（转发无参版本）。
void CPCRoomWorldDropEvent2nd::StartEvent(Word_Param param)
{
    // [N7 封顶] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换，探针不可翻。
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c068：清事件标志并打日志（__PRETTY_FUNCTION__，line 0x1b）。
void CPCRoomWorldDropEvent2nd::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1b, 0);
    tr("PCRoom World Drop Event 2nd End!");
}
