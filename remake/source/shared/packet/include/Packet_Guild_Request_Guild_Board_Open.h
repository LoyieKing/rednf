//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_REQUEST_GUILD_BOARD_OPEN_H
#define PACKET_GUILD_REQUEST_GUILD_BOARD_OPEN_H

#include "PacketHeader.h"

class Packet_Guild_Request_Guild_Board_Open : public PacketHeader {
public:
    Packet_Guild_Request_Guild_Board_Open(): PacketHeader(0x2329,0x13) {};
};


#endif //PACKET_GUILD_REQUEST_GUILD_BOARD_OPEN_H
