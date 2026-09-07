// df_game_r U/X Game Log 事件 CUXGameLogEvent 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写；地址见头文件注释）：
//   C1 0x08117678 / D1 0x08117694 / D0 0x081176c4 /
//   StartEvent() 0x081176e2 / StartEvent(Word_Param) 0x08117734 /
//   EndEvent 0x0811774e（均弱符号 W，独立 TU 定义）。
#include "CUXGameLogEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x08117678：仅基类构造 + vptr 置位（无自有成员初始化）。
CUXGameLogEvent::CUXGameLogEvent()
{
}

// ORIG 0x08117694 / 0x081176c4：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CUXGameLogEvent::~CUXGameLogEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x081176e2：SetEventFlag(true) + cMyTrace("CUXGameLogEvent::StartEvent()",0x13,0)
// ("CUXGameLogEvent Event Start!")。
void CUXGameLogEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr("CUXGameLogEvent::StartEvent()", 0x13, 0);
    tr("CUXGameLogEvent Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08117734：thunk——经 vtable slot 2（StartEvent()）虚调用。
void CUXGameLogEvent::StartEvent(Word_Param param)
{
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x0811774e：SetEventFlag(false) + cMyTrace("CUXGameLogEvent::EndEvent()",0x1e,0)
// ("CUXGameLogEvent Event End!")。
void CUXGameLogEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr("CUXGameLogEvent::EndEvent()", 0x1e, 0);
    tr("CUXGameLogEvent Event End!");
}
