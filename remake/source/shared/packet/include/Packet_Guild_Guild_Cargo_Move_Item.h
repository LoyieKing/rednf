//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_GUILD_CARGO_MOVE_ITEM_H
#define PACKET_GUILD_GUILD_CARGO_MOVE_ITEM_H

#include "PacketHeader.h"

class Packet_Guild_Guild_Cargo_Move_Item : public PacketHeader {
public:
    Packet_Guild_Guild_Cargo_Move_Item(): PacketHeader(0x70f,0x22) {};
};


#endif //PACKET_GUILD_GUILD_CARGO_MOVE_ITEM_H
