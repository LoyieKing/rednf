//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_P2P_STATISTICS_H
#define PACKET_P2P_STATISTICS_H

#include "PacketHeader.h"

class Packet_P2P_Statistics : public PacketHeader {
public:
    int m_fieldA;             // 0xa
    int m_fieldB;             // 0xe
    unsigned char m_fieldC;   // 0x12
    unsigned short m_fieldD;  // 0x13
    unsigned short m_fieldE;  // 0x15
    unsigned short m_fieldF;  // 0x17
    int m_fieldG;             // 0x19
    int m_fieldH;             // 0x1d
    int m_fieldI;             // 0x21
    int m_fieldJ;             // 0x25
    unsigned short m_fieldK;  // 0x29
    unsigned short m_fieldL;  // 0x2b
    unsigned short m_fieldM;  // 0x2d
    int m_fieldN;             // 0x2f
    int m_fieldO;             // 0x33
    int m_fieldP;             // 0x37
    int m_fieldQ;             // 0x3b
    Packet_P2P_Statistics() : PacketHeader(0x27e1, 0x3f)
    {
        m_fieldA = 0;
        m_fieldB = 0;
        m_fieldC = 0;
        m_fieldD = 0;
        m_fieldE = 0;
        m_fieldF = 0;
        m_fieldG = 0;
        m_fieldH = 0;
        m_fieldI = 0;
        m_fieldJ = 0;
        m_fieldK = 0;
        m_fieldL = 0;
        m_fieldM = 0;
        m_fieldN = 0;
        m_fieldO = 0;
        m_fieldP = 0;
        m_fieldQ = 0;
    }
} __attribute__((packed));

#endif  // PACKET_P2P_STATISTICS_H
