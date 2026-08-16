//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_ATTENDANCE_INFO_H
#define PACKET_GUILD_ATTENDANCE_INFO_H

#include "PacketHeader.h"

class Packet_Guild_Attendance_Info : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa
    unsigned int m_charNo;      // +0xe
    unsigned char m_field12;    // +0x12
    Packet_Guild_Attendance_Info() : PacketHeader(0x1bc6, 0x13),
        m_guildKey(0), m_charNo(0), m_field12(0)
    {

    };
} __attribute__((packed));

#endif  // PACKET_GUILD_ATTENDANCE_INFO_H
