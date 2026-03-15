//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_CREATE_GUILD_AGIT_H
#define PACKET_GUILD_CREATE_GUILD_AGIT_H

#include "PacketHeader.h"

class Packet_Guild_Create_Guild_Agit : public PacketHeader {
public:
    Packet_Guild_Create_Guild_Agit() : PacketHeader(0x6dc, 0x1a){};
};

#endif  // PACKET_GUILD_CREATE_GUILD_AGIT_H
