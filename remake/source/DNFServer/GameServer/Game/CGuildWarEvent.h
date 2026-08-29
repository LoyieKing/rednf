#ifndef GAME_CGUILDWAREVENT_H_
#define GAME_CGUILDWAREVENT_H_

//   CEventBase（vptr +0x00 / stADDINFO +0x04 / m_isEventing +0x08，sizeof 0xc）
//   unsigned short m_duration +0x0a（持续分钟数，默认 5；复用基类尾 padding）
//   unsigned short m_field0c   +0x0c
//   sizeof = 0x10（ORIG new 两处 _Znwj(0x10)：0x08114f9d/0x0811ad0c）。
// 虚表 0x08b553a0：D1/D0 @0,1 / StartEvent @2 / EndEvent @3 /
//   StartEvent(Word_Param) @4，其余继承 CEventBase。
//   CEventBase（vptr +0x00 / m_isEventing +0x04，sizeof 0xa）
//   unsigned short m_duration +0x0a（持续分钟数，默认 5）
//   unsigned short m_field0c   +0x0c
//   sizeof = 0xe。
// 虚表 0x08b553a8：D1/D0 @0,1 / StartEvent @2 / EndEvent @3 /
//   StartEvent(Word_Param) @4，其余继承 CEventBase。

#include "CGuildServerProxy.h"   // CEventBase / Word_Param / CGuildServerProxy

class CGuildWarEvent : public CEventBase
{
public:
    CGuildWarEvent();
    virtual ~CGuildWarEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);

    unsigned short m_duration;   // +0x0a（分钟）
    unsigned short m_field0c;    // +0x0c
};

#endif  // GAME_CGUILDWAREVENT_H_
