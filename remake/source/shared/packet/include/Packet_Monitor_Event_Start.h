//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_EVENT_START_H
#define PACKET_MONITOR_EVENT_START_H

#include "PacketHeader.h"

class Packet_Monitor_Event_Start : public PacketHeader {
public:
    Packet_Monitor_Event_Start() : PacketHeader(0x44f, 0x12){};
    unsigned int m_eventCode;     // +0xa
    unsigned short m_eventParam1; // +0xe
    unsigned short m_eventParam2; // +0x10
} __attribute__((packed));

#endif  // PACKET_MONITOR_EVENT_START_H
