//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_NO_CACHE_H
#define PACKET_NO_CACHE_H

#include "PacketHeader.h"

class Packet_No_Cache : public PacketHeader {
public:
    Packet_No_Cache() : PacketHeader(0x1b6d, 0x16){};
};

#endif  // PACKET_NO_CACHE_H
