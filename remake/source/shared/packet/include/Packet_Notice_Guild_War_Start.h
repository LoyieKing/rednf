//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_NOTICE_GUILD_WAR_START_H
#define PACKET_NOTICE_GUILD_WAR_START_H

#include "PacketHeader.h"

class Packet_Notice_Guild_War_Start : public PacketHeader {
public:
    Packet_Notice_Guild_War_Start() : PacketHeader(0x416, 0xb){};
};

#endif  // PACKET_NOTICE_GUILD_WAR_START_H
