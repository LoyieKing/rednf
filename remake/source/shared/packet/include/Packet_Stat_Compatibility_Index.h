//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_STAT_COMPATIBILITY_INDEX_H
#define PACKET_STAT_COMPATIBILITY_INDEX_H

#include "PacketHeader.h"

class Packet_Stat_Compatibility_Index : public PacketHeader {
public:
    int m_fieldA;  // +a
    char m_fieldB;  // +e
    char m_fieldC;  // +f
    int m_fieldD;  // +10
    short m_fieldE;  // +14
    short m_fieldF;  // +16
    short m_fieldG;  // +18
    short m_fieldH;  // +1a
    char m_fieldI;  // +1c
    char m_fieldJ;  // +1d

    Packet_Stat_Compatibility_Index() : PacketHeader(0x27e0, 0x1e),
        m_fieldA(0), m_fieldB(0), m_fieldC(0), m_fieldD(0), m_fieldE(0), m_fieldF(0), m_fieldG(0), m_fieldH(0), m_fieldI(0), m_fieldJ(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Stat_Compatibility_Index, 0x1e);


#endif //PACKET_STAT_COMPATIBILITY_INDEX_H
