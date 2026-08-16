//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_STDUPDATE_H
#define PACKET_HADES_STDUPDATE_H

#include "PacketHeader.h"

class Packet_Hades_StdUpdate : public PacketHeader {
public:
    int m_fieldA;  // +a
    short m_fieldB;  // +e
    short m_fieldC;  // +10

    Packet_Hades_StdUpdate() : PacketHeader(0xfd9, 0x12),
        m_fieldA(0), m_fieldB(0), m_fieldC(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Hades_StdUpdate, 0x12);

#endif  // PACKET_HADES_STDUPDATE_H
