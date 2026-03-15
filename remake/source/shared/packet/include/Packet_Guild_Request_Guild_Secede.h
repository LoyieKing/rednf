//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_REQUEST_GUILD_SECEDE_H
#define PACKET_GUILD_REQUEST_GUILD_SECEDE_H

#include "PacketHeader.h"

class Packet_Guild_Request_Guild_Secede : public PacketHeader {
public:
    Packet_Guild_Request_Guild_Secede(): PacketHeader(0x439,0x34) {};
};


#endif //PACKET_GUILD_REQUEST_GUILD_SECEDE_H
