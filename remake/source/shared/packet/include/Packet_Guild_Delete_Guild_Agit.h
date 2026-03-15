//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_DELETE_GUILD_AGIT_H
#define PACKET_GUILD_DELETE_GUILD_AGIT_H

#include "PacketHeader.h"

class Packet_Guild_Delete_Guild_Agit : public PacketHeader {
public:
    Packet_Guild_Delete_Guild_Agit() : PacketHeader(0x6de, 0x12){};
};

#endif  // PACKET_GUILD_DELETE_GUILD_AGIT_H
