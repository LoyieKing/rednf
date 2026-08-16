//
// Created from df_game_r: Packet_Mannerless_User_Accusation::Packet_Mannerless_User_Accusation()
// @ 0x822c1e6（Ghidra 反编译）
//

#ifndef PACKET_MANNERLESS_USER_ACCUSATION_H
#define PACKET_MANNERLESS_USER_ACCUSATION_H

#include <string.h>

#include "PacketHeader.h"

class Packet_Mannerless_User_Accusation : public PacketHeader {
public:
    unsigned int m_uid;   // +0xa（ctor = 0xffffffff）
    int m_characNo;            // +0xe
    int m_fieldC;            // +0x12（ctor = 4）
    char m_pad16[0x8];       // +0x16..+0x1d（ctor 未触碰/布局待定）
    int m_accId;            // +0x1e
    char m_field22[0x1e];    // +0x22..+0x3f（ctor memset 区域）
    char m_field40[0x1e];    // +0x40..+0x5d（ctor memset 区域）
    char m_field5e[0x100];   // +0x5e..+0x15d（ctor memset 区域）
    char m_field15e;      // +0x15e

    Packet_Mannerless_User_Accusation() : PacketHeader(0x1b66, 0x15f),
        m_fieldA(0xffffffff), m_fieldB(0), m_fieldC(4), m_fieldD(0)
    {
        memset(m_field22, 0, sizeof(m_field22));
        memset(m_field40, 0, sizeof(m_field40));
        memset(m_field5e, 0, sizeof(m_field5e));
        m_field15e = 0;
    }
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Mannerless_User_Accusation, 0x15f);

#endif  // PACKET_MANNERLESS_USER_ACCUSATION_H
