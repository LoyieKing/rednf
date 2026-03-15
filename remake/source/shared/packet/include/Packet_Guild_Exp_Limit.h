//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_EXP_LIMIT_H
#define PACKET_GUILD_EXP_LIMIT_H

#include "PacketHeader.h"

class Packet_Guild_Exp_Limit : public PacketHeader {
public:
    Packet_Guild_Exp_Limit() : PacketHeader(0x6e5, 0x5a){};
};

#endif  // PACKET_GUILD_EXP_LIMIT_H
