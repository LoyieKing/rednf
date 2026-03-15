//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_GUILD_CARGO_PUSH_ITEM_H
#define PACKET_GUILD_GUILD_CARGO_PUSH_ITEM_H

#include "PacketHeader.h"

class Packet_Guild_Guild_Cargo_Push_Item : public PacketHeader {
public:
    Packet_Guild_Guild_Cargo_Push_Item(): PacketHeader(0x70d,0x53) {};
};


#endif //PACKET_GUILD_GUILD_CARGO_PUSH_ITEM_H
