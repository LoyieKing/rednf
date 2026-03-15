//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_INCREASE_DECREASE_POWER_WAR_POINT_H
#define PACKET_GUILD_INCREASE_DECREASE_POWER_WAR_POINT_H

#include "PacketHeader.h"

class Packet_Guild_Increase_Decrease_Power_War_Point : public PacketHeader {
public:
    Packet_Guild_Increase_Decrease_Power_War_Point(): PacketHeader(0x6db,0x16) {};
};


#endif //PACKET_GUILD_INCREASE_DECREASE_POWER_WAR_POINT_H
