//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CHANGE_GUILD_NAME_H
#define PACKET_CHANGE_GUILD_NAME_H

#include "PacketHeader.h"

class Packet_Change_Guild_Name : public PacketHeader {
public:
    Packet_Change_Guild_Name() : PacketHeader(0x444, 0x29){};
};

#endif  // PACKET_CHANGE_GUILD_NAME_H
