//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_GUILD_CREATE_H
#define PACKET_REQUEST_GUILD_CREATE_H

#include "PacketHeader.h"

class Packet_Request_Guild_Create : public PacketHeader {
public:
    Packet_Request_Guild_Create() : PacketHeader(0x43f, 0x32){};
};

#endif  // PACKET_REQUEST_GUILD_CREATE_H
