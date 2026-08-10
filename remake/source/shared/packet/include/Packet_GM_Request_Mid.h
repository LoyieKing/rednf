//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GM_REQUEST_MID_H
#define PACKET_GM_REQUEST_MID_H

#include <string.h>

#include "PacketHeader.h"

class Packet_GM_Request_Mid : public PacketHeader {
public:
    Packet_GM_Request_Mid() : PacketHeader(0x76c, 0x34)
    {
        memset((char*)this + 0x16, 0, 0x1e);
    }
} __attribute__((packed));

#endif  // PACKET_GM_REQUEST_MID_H
