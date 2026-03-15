//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_GUILD_CARGO_POP_ITEM_H
#define PACKET_GUILD_GUILD_CARGO_POP_ITEM_H

#include "PacketHeader.h"

class Packet_Guild_Guild_Cargo_Pop_Item : public PacketHeader {
public:
    Packet_Guild_Guild_Cargo_Pop_Item(): PacketHeader(0x70e,0x23) {};
};


#endif //PACKET_GUILD_GUILD_CARGO_POP_ITEM_H
