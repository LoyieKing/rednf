//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_GUILD_CARGO_MOVE_ITEM_H
#define PACKET_GUILD_GUILD_CARGO_MOVE_ITEM_H

#include "PacketHeader.h"

class Packet_Guild_Guild_Cargo_Move_Item : public PacketHeader {
public:
    unsigned int m_guildKey;  // +0xa
    unsigned int m_charNo;    // +0xe
    int m_fromSlot;           // +0x12
    int m_toSlot;             // +0x16
    int m_count;              // +0x1a
    int m_type;               // +0x1e
    Packet_Guild_Guild_Cargo_Move_Item(): PacketHeader(0x70f,0x22) {};
} __attribute__((packed));


#endif //PACKET_GUILD_GUILD_CARGO_MOVE_ITEM_H
