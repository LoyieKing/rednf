//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_REQUEST_GUILD_MASTER_DELEGATE_H
#define PACKET_GUILD_REQUEST_GUILD_MASTER_DELEGATE_H

#include "PacketHeader.h"

class Packet_Guild_Request_Guild_Master_Delegate : public PacketHeader {
public:
    unsigned int m_guildKey;         // +0xa
    unsigned int m_requesterCharNo;  // +0xe
    char m_name[0x1e];               // +0x12
    Packet_Guild_Request_Guild_Master_Delegate(): PacketHeader(0x43b,0x30) {};
} __attribute__((packed));


#endif //PACKET_GUILD_REQUEST_GUILD_MASTER_DELEGATE_H
