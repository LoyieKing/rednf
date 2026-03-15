//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_APPLY_ORIGIAL_POWER_SIDE_H
#define PACKET_GUILD_APPLY_ORIGIAL_POWER_SIDE_H

#include "PacketHeader.h"

class Packet_Guild_Apply_Origial_Power_Side : public PacketHeader {
public:
    Packet_Guild_Apply_Origial_Power_Side(): PacketHeader(0x6b2,0x13) {};
};


#endif //PACKET_GUILD_APPLY_ORIGIAL_POWER_SIDE_H
