//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_UDP_HEARTBEAT_H
#define PACKET_MONITOR_UDP_HEARTBEAT_H

#include "PacketHeader.h"

class Packet_Monitor_UDP_HeartBeat : public PacketHeader {
public:
    unsigned char m_channel;  // +0xa  0xc8=DB, 1..0xbe=游戏服
    Packet_Monitor_UDP_HeartBeat() : PacketHeader(0x3ec, 0xb)
    {
        m_channel = (char)0xff;
    }
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_UDP_HeartBeat, 0xb);

#endif  // PACKET_MONITOR_UDP_HEARTBEAT_H
