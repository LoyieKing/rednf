//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PCROOMPLAYTIMEREWARD_H
#define PACKET_PCROOMPLAYTIMEREWARD_H

#include "PacketHeader.h"

class Packet_PcRoomPlayTimeReward : public PacketHeader {
public:
    char m_fieldA;  // +a
    int m_fieldB;  // +b
    int m_fieldC;  // +f

    Packet_PcRoomPlayTimeReward() : PacketHeader(0x27ec, 0x13),
        m_fieldA(0), m_fieldB(0), m_fieldC(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_PcRoomPlayTimeReward, 0x13);

#endif  // PACKET_PCROOMPLAYTIMEREWARD_H
