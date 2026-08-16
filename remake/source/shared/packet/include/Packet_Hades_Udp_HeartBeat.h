//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_UDP_HEARTBEAT_H
#define PACKET_HADES_UDP_HEARTBEAT_H

#include "PacketHeader.h"

class Packet_Hades_Udp_HeartBeat : public PacketHeader {
public:
    short m_fieldA;  // +a
    short m_fieldB;  // +c

    Packet_Hades_Udp_HeartBeat() : PacketHeader(0xfd8, 0xe),
        m_fieldA(0), m_fieldB(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Hades_Udp_HeartBeat, 0xe);

#endif  // PACKET_HADES_UDP_HEARTBEAT_H
