//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_CALL_GUILD_INVITE_H
#define PACKET_GUILD_CALL_GUILD_INVITE_H

#include "PacketHeader.h"

class Packet_Guild_Call_Guild_Invite : public PacketHeader {
public:
    Packet_Guild_Call_Guild_Invite() : PacketHeader(0x434, 0x30){};
};

#endif  // PACKET_GUILD_CALL_GUILD_INVITE_H
