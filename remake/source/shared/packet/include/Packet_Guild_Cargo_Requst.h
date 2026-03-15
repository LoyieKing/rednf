//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_CARGO_REQUST_H
#define PACKET_GUILD_CARGO_REQUST_H

#include "PacketHeader.h"

class Packet_Guild_Cargo_Requst : public PacketHeader {
public:
    Packet_Guild_Cargo_Requst() : PacketHeader(0x70a, 0x12){};
};

#endif  // PACKET_GUILD_CARGO_REQUST_H
