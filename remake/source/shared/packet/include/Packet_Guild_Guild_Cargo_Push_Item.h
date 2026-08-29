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
    int m_b;                       // +0x12（result/dword 视图；InterWave1 W0）
    int m_d;                       // +0x16
    unsigned char m_itemType;      // +0x1a
    char m_item[0x35];             // +0x1b（DnfItemInfo）
    unsigned char m_e;             // +0x50
    unsigned short m_f;            // +0x51
    // ORIG ctor 0x084e9434（定义见 InterDispatcher.cpp，需完整 DnfItemInfo 构造；
    // 与 Guild 域同名类冲突故外置，placement-new 显式构造 m_item）。
    Packet_Guild_Guild_Cargo_Push_Item();
} __attribute__((packed));


#endif //PACKET_GUILD_GUILD_CARGO_PUSH_ITEM_H
