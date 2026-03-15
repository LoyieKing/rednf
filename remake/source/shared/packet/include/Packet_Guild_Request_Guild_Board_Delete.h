//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_REQUEST_GUILD_BOARD_DELETE_H
#define PACKET_GUILD_REQUEST_GUILD_BOARD_DELETE_H

#include "PacketHeader.h"

class Packet_Guild_Request_Guild_Board_Delete : public PacketHeader {
public:
    Packet_Guild_Request_Guild_Board_Delete(): PacketHeader(0x2331,0x16) {};
};


#endif //PACKET_GUILD_REQUEST_GUILD_BOARD_DELETE_H
