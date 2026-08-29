// ORIG vtable 0x08b314e0（0/1 虚析构，2 = StartEvent() 0x080e94dc 空体，
// 3 = EndEvent() 0x080e9534，4 = StartEvent(Word_Param) 0x080e94e2 完整实现；
// C1/D1/D0 为弱符号 0x08116b2c/0x080e95e2/0x080e9612）。
// 布局：仅 CEventBase 基区（vptr +0x00 / stADDINFO +0x04 / m_isEventing +0x08，
// sizeof 0x0c），无自有成员（C1 仅基类构造 + vptr，objdump 实证）。
// ORIG vtable 0x08b4f498（0/1 虚析构，2 = StartEvent() 0x080e94dc 空体，
// 3 = EndEvent() 0x080e9534，4 = StartEvent(Word_Param) 0x080e94e2 完整实现；
// C1/D1/D0 为弱符号 0x08116b2c/0x080e95e2/0x080e9612）。
// 布局：仅 CEventBase 基区（vptr +0x00 / stADDINFO +0x04 / m_isEventing +0x08，
// sizeof 0x0c），无自有成员（C1 仅基类构造 + vptr，objdump 实证）。
#ifndef DNF_GAME_CEVENTADVANCEALTAROPEN_H_
#define DNF_GAME_CEVENTADVANCEALTAROPEN_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CEventAdvanceAltarOpen : public CEventBase
{
public:
    CEventAdvanceAltarOpen();
    virtual ~CEventAdvanceAltarOpen();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CEVENTADVANCEALTAROPEN_H_
