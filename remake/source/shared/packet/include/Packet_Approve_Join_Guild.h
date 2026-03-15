//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_APPROVE_JOIN_GUILD_H
#define PACKET_APPROVE_JOIN_GUILD_H

#include "PacketHeader.h"

class Packet_Approve_Join_Guild : public PacketHeader {
public:
    Packet_Approve_Join_Guild() : PacketHeader(0x1bc2, 0x16){};
};

#endif  // PACKET_APPROVE_JOIN_GUILD_H
