//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PVPCHANNELUSERCOUNT_H
#define PACKET_PVPCHANNELUSERCOUNT_H

#include "PacketHeader.h"

class Packet_PvPChannelUserCount : public PacketHeader {
public:
    unsigned int m_charNo;          // +0xa  FindUser_CharNo
    unsigned int m_fieldE;          // +0xe
    unsigned int m_field12;         // +0x12
    int m_countA;                   // +0x16
    int m_countB;                   // +0x1a
    int m_pad1e;                    // +0x1e
    int m_countC;                   // +0x22
    int m_countD;                   // +0x26
    Packet_PvPChannelUserCount() : PacketHeader(0xfa3, 0x2a){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_PvPChannelUserCount, 0x2a);

#endif  // PACKET_PVPCHANNELUSERCOUNT_H
