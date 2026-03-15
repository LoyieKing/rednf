//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_ADD_GUILD_FUND_H
#define PACKET_ADD_GUILD_FUND_H

#include "PacketHeader.h"

class Packet_Add_Guild_Fund : public PacketHeader {
public:
    Packet_Add_Guild_Fund() : PacketHeader(0x1bbc, 0x17){};
};

#endif  // PACKET_ADD_GUILD_FUND_H
