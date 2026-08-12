//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_EVENT_END_H
#define PACKET_MONITOR_EVENT_END_H

#include "PacketHeader.h"

class Packet_Monitor_Event_End : public PacketHeader {
public:
    Packet_Monitor_Event_End() : PacketHeader(0x450, 0xe){};
    int m_fieldA;  // +0xa（ORIG：TranslateSignal case3 直写 +0xa，总 0xe）
} __attribute__((packed));

#endif  // PACKET_MONITOR_EVENT_END_H
