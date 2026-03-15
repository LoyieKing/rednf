//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_REPLY_GUILD_INVITE_FROM_INVITED_H
#define PACKET_GUILD_REPLY_GUILD_INVITE_FROM_INVITED_H

#include "PacketHeader.h"

class Packet_Guild_Reply_Guild_Invite_From_Invited : public PacketHeader {
public:
    Packet_Guild_Reply_Guild_Invite_From_Invited(): PacketHeader(0x436,0xf) {};
};


#endif //PACKET_GUILD_REPLY_GUILD_INVITE_FROM_INVITED_H
