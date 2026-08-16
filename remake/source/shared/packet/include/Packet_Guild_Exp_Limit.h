//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_EXP_LIMIT_H
#define PACKET_GUILD_EXP_LIMIT_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Guild_Exp_Limit : public PacketHeader {
public:
    int m_fieldA;  // +a
    int m_fieldB;  // +e
    int m_fieldC;  // +12
    char m_field16[0x44];  // +16..+59（ctor memset 区域）

    Packet_Guild_Exp_Limit() : PacketHeader(0x6e5, 0x5a),
        m_fieldA(0), m_fieldB(0), m_fieldC(0)
    {
    memset(m_field16, 0, sizeof(m_field16));
    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Guild_Exp_Limit, 0x5a);

#endif  // PACKET_GUILD_EXP_LIMIT_H
