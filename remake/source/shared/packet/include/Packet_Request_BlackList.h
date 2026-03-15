//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_BLACKLIST_H
#define PACKET_REQUEST_BLACKLIST_H

#include "PacketHeader.h"

class Packet_Request_BlackList : public PacketHeader {
public:
    Packet_Request_BlackList() : PacketHeader(0x5e0, 0x12){};
};

#endif  // PACKET_REQUEST_BLACKLIST_H
