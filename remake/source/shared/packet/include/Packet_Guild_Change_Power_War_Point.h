//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_CHANGE_POWER_WAR_POINT_H
#define PACKET_GUILD_CHANGE_POWER_WAR_POINT_H

#include "PacketHeader.h"

class Packet_Guild_Change_Power_War_Point : public PacketHeader {
public:
    Packet_Guild_Change_Power_War_Point(): PacketHeader(0x6a4,0x2d) {};
};


#endif //PACKET_GUILD_CHANGE_POWER_WAR_POINT_H
