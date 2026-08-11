//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_EVENT_START_H
#define PACKET_MONITOR_EVENT_START_H

#include "PacketHeader.h"

class Packet_Monitor_Event_Start : public PacketHeader {
public:
    Packet_Monitor_Event_Start() : PacketHeader(0x44f, 0x12){};
    int m_fieldA;              // +0xa
    unsigned short m_fieldE;   // +0xe
    unsigned short m_field10;  // +0x10
} __attribute__((packed));

#endif  // PACKET_MONITOR_EVENT_START_H
