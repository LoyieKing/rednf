//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_REQUEST_GUILD_SECEDE_H
#define PACKET_GUILD_REQUEST_GUILD_SECEDE_H

#include "PacketHeader.h"

class Packet_Guild_Request_Guild_Secede : public PacketHeader {
public:
    unsigned int m_guildKey;         // +0xa
    unsigned int m_requesterCharNo;  // +0xe
    unsigned int m_nameLen;          // +0x12
    char m_name[0x1e];               // +0x16
    Packet_Guild_Request_Guild_Secede(): PacketHeader(0x439,0x34) {};
} __attribute__((packed));


#endif //PACKET_GUILD_REQUEST_GUILD_SECEDE_H
