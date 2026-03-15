//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_WRITE_GUILD_MEMBER_MEMO_H
#define PACKET_GUILD_WRITE_GUILD_MEMBER_MEMO_H

#include "PacketHeader.h"

class Packet_Guild_Write_Guild_Member_Memo : public PacketHeader {
public:
    Packet_Guild_Write_Guild_Member_Memo(): PacketHeader(0x4d1,0x27) {};
};


#endif //PACKET_GUILD_WRITE_GUILD_MEMBER_MEMO_H
