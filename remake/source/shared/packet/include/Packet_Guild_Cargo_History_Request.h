//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_CARGO_HISTORY_REQUEST_H
#define PACKET_GUILD_CARGO_HISTORY_REQUEST_H

#include "PacketHeader.h"

class Packet_Guild_Cargo_History_Request : public PacketHeader {
public:
    Packet_Guild_Cargo_History_Request(): PacketHeader(0x70b,0x12) {};
};


#endif //PACKET_GUILD_CARGO_HISTORY_REQUEST_H
