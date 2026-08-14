//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_GUILD_CHAT_HYPER_LINK_H
#define PACKET_MONITOR_GUILD_CHAT_HYPER_LINK_H

#include "PacketHeader.h"

class Packet_Monitor_Guild_Chat_Hyper_Link : public PacketHeader {
public:
    unsigned int m_charNo;    // +0xa
    unsigned int m_guildKey;  // +0xe
    unsigned char m_field_12; // +0x12
    char m_items[0x138];      // +0x13
    unsigned char m_msgLen;   // +0x14b
    char m_msg[0x100];        // +0x14c
    Packet_Monitor_Guild_Chat_Hyper_Link(): PacketHeader(0x2718,0x24c) {};
} __attribute__((packed));


#endif //PACKET_MONITOR_GUILD_CHAT_HYPER_LINK_H
