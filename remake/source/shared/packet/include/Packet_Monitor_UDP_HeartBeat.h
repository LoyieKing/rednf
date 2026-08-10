//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_UDP_HEARTBEAT_H
#define PACKET_MONITOR_UDP_HEARTBEAT_H

#include "PacketHeader.h"

class Packet_Monitor_UDP_HeartBeat : public PacketHeader {
public:
    unsigned char m_fieldA;  // 0xa
    Packet_Monitor_UDP_HeartBeat() : PacketHeader(0x3ec, 0xb)
    {
        m_fieldA = (char)0xff;
    }
} __attribute__((packed));

#endif  // PACKET_MONITOR_UDP_HEARTBEAT_H
