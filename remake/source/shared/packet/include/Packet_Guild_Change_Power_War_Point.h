//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_CHANGE_POWER_WAR_POINT_H
#define PACKET_GUILD_CHANGE_POWER_WAR_POINT_H

#include <vector>

#include "PacketHeader.h"
#include <string.h>

class Packet_Guild_Change_Power_War_Point : public PacketHeader {
public:
    Packet_Guild_Change_Power_War_Point() : PacketHeader(0x6a4, 0x2d),
        m_fieldA(0), m_field2B(0), m_field2C(0)
    {
    memset((char*)this + 0xb, 0, 0x10);
    memset((char*)this + 0x1b, 0, 0x10);
    };
    void GetUserList(std::vector<unsigned int>& list);
    unsigned char m_fieldA;        // +0xa
    unsigned int m_fieldB;         // +0xb
    unsigned int m_fieldF;         // +0xf
    unsigned int m_field13;        // +0x13
    unsigned int m_field17;        // +0x17
    unsigned int m_field1B;        // +0x1b
    unsigned int m_field1F;        // +0x1f
    unsigned int m_field23;        // +0x23
    unsigned int m_field27;        // +0x27
    unsigned char m_field2B;       // +0x2b
    unsigned char m_field2C;       // +0x2c
} __attribute__((packed));


#endif //PACKET_GUILD_CHANGE_POWER_WAR_POINT_H
