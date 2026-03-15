//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_SERVER_MATCH_DATA_H
#define PACKET_SERVER_MATCH_DATA_H

#include "PacketHeader.h"

class Packet_Server_Match_data : public PacketHeader {
public:
    Packet_Server_Match_data() : PacketHeader(0x1b67, 0xd){};
};

#endif  // PACKET_SERVER_MATCH_DATA_H
