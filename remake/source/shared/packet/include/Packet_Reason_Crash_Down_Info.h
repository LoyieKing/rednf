//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REASON_CRASH_DOWN_INFO_H
#define PACKET_REASON_CRASH_DOWN_INFO_H

#include "PacketHeader.h"

class Packet_Reason_Crash_Down_Info : public PacketHeader {
public:
    int m_fieldA;  // +a
    int m_fieldB;  // +e
    int m_fieldC;  // +12
    int m_fieldD;  // +16

    Packet_Reason_Crash_Down_Info() : PacketHeader(0xc48, 0x1a),
        m_fieldA(0), m_fieldB(0), m_fieldC(0), m_fieldD(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Reason_Crash_Down_Info, 0x1a);

#endif  // PACKET_REASON_CRASH_DOWN_INFO_H
