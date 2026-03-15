//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_NOTICE_GUILD_WAR_POINT_CHANGE_H
#define PACKET_NOTICE_GUILD_WAR_POINT_CHANGE_H

#include "PacketHeader.h"

class Packet_Notice_Guild_War_Point_Change : public PacketHeader {
public:
    Packet_Notice_Guild_War_Point_Change(): PacketHeader(0x417,0xf) {};
};


#endif //PACKET_NOTICE_GUILD_WAR_POINT_CHANGE_H
