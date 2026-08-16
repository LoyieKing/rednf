//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PCROOM_RESPONSE_H
#define PACKET_PCROOM_RESPONSE_H

#include "PacketHeader.h"

class Packet_Pcroom_Response : public PacketHeader {
public:
    int m_fieldA;  // +a
    int m_fieldB;  // +e
    char m_pad12[0xc];  // +12..+1d（ctor 未触碰/布局待定）
    char m_fieldC;  // +1e

    Packet_Pcroom_Response() : PacketHeader(0xfaf, 0x1f),
        m_fieldA(0), m_fieldB(0), m_fieldC(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Pcroom_Response, 0x1f);

#endif  // PACKET_PCROOM_RESPONSE_H
