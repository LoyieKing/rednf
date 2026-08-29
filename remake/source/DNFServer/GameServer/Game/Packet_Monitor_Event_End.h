// ORIG 弱内联 ctor W _ZN23Packet_Monitor_Event_EndC1Ev @0x0826889a (sz 0x23)：
//   PacketHeader(0x0450, 0xe)
//   无派生字段初始化指令（仅基类构造；+0x0a 事件码由调用方填写）。
// 布局 0xe = 0x0a + 4；GmCmds_C.cpp CEndEvent::execute 消费 m_eventCode。
#ifndef DNF_GAME_PACKET_MONITOR_EVENT_END_H_
#define DNF_GAME_PACKET_MONITOR_EVENT_END_H_

#include "PacketHeader.h"

class Packet_Monitor_Event_End : public PacketHeader
{
public:
    Packet_Monitor_Event_End()
        : PacketHeader(0x0450, 0xe)
    {
    }
    unsigned int m_eventCode;    // +0x0a（ORIG ctor 不清零，调用方填写）
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Event_End, 0xe)
#endif  // DNF_GAME_PACKET_MONITOR_EVENT_END_H_
