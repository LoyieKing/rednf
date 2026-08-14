//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_GUILD_CARGO_PUSH_ITEM_H
#define PACKET_GUILD_GUILD_CARGO_PUSH_ITEM_H

#include "PacketHeader.h"

class Packet_Guild_Guild_Cargo_Push_Item : public PacketHeader {
public:
    unsigned int m_guildKey;       // +0xa
    unsigned int m_charNo;         // +0xe
    int m_slot;                    // +0x12
    int m_count;                   // +0x16
    unsigned char m_itemType;      // +0x1a
    char m_item[0x35];             // +0x1b（DnfItemInfo）
    unsigned char m_fst;           // +0x50
    unsigned short m_fsn;          // +0x51
    Packet_Guild_Guild_Cargo_Push_Item(): PacketHeader(0x70d,0x53) {};
} __attribute__((packed));


#endif //PACKET_GUILD_GUILD_CARGO_PUSH_ITEM_H
