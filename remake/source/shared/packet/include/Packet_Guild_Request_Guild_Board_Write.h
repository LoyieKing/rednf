//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_REQUEST_GUILD_BOARD_WRITE_H
#define PACKET_GUILD_REQUEST_GUILD_BOARD_WRITE_H

#include "PacketHeader.h"

class Packet_Guild_Request_Guild_Board_Write : public PacketHeader {
public:
    Packet_Guild_Request_Guild_Board_Write(): PacketHeader(0x232d,0x90) {};
};


#endif //PACKET_GUILD_REQUEST_GUILD_BOARD_WRITE_H
