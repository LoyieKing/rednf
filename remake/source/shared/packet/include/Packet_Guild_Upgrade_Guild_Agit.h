//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_UPGRADE_GUILD_AGIT_H
#define PACKET_GUILD_UPGRADE_GUILD_AGIT_H

#include "PacketHeader.h"

class Packet_Guild_Upgrade_Guild_Agit : public PacketHeader {
public:
    Packet_Guild_Upgrade_Guild_Agit() : PacketHeader(0x6e3, 0x1a){};
};

#endif  // PACKET_GUILD_UPGRADE_GUILD_AGIT_H
