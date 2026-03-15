//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_DEBUG_H
#define PACKET_GUILD_DEBUG_H

#include "PacketHeader.h"

class Packet_Guild_Debug : public PacketHeader {
public:
    Packet_Guild_Debug() : PacketHeader(0x1bc9, 0x1e){};
};

#endif  // PACKET_GUILD_DEBUG_H
