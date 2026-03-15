//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_GUILD_CARGO_UPGRADE_H
#define PACKET_GUILD_GUILD_CARGO_UPGRADE_H

#include "PacketHeader.h"

class Packet_Guild_Guild_Cargo_Upgrade : public PacketHeader {
public:
    Packet_Guild_Guild_Cargo_Upgrade(): PacketHeader(0x713,0x1e) {};
};


#endif //PACKET_GUILD_GUILD_CARGO_UPGRADE_H
