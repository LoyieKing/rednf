//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_GUILD_CARGO_POP_ITEM_H
#define PACKET_GUILD_GUILD_CARGO_POP_ITEM_H

#include "PacketHeader.h"

class Packet_Guild_Guild_Cargo_Pop_Item : public PacketHeader {
public:
    unsigned int m_guildKey;   // +0xa
    unsigned int m_charNo;     // +0xe
    int m_slot;                // +0x12
    int m_id;                  // +0x16
    int m_count;               // +0x1a
    unsigned char m_itemType;  // +0x1e
    unsigned int m_uit;        // +0x1f
    Packet_Guild_Guild_Cargo_Pop_Item(): PacketHeader(0x70e,0x23) {};
} __attribute__((packed));


#endif //PACKET_GUILD_GUILD_CARGO_POP_ITEM_H
