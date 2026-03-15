//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_NOTICE_GUILD_WAR_INFO_H
#define PACKET_NOTICE_GUILD_WAR_INFO_H

#include "PacketHeader.h"

class Packet_Notice_Guild_War_Info : public PacketHeader {
public:
    Packet_Notice_Guild_War_Info() : PacketHeader(0x414, 0x15c){};
};

#endif  // PACKET_NOTICE_GUILD_WAR_INFO_H
