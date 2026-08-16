//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_INCREASE_DECREASE_POWER_WAR_POINT_H
#define PACKET_GUILD_INCREASE_DECREASE_POWER_WAR_POINT_H

#include "PacketHeader.h"

class Packet_Guild_Increase_Decrease_Power_War_Point : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa
    unsigned int m_charNo;      // +0xe
    int m_point;                // +0x12
    Packet_Guild_Increase_Decrease_Power_War_Point() : PacketHeader(0x6db, 0x16),
        m_guildKey(0), m_charNo(0), m_point(0)
    {

    };
} __attribute__((packed));


#endif //PACKET_GUILD_INCREASE_DECREASE_POWER_WAR_POINT_H
