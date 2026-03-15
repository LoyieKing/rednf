//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GM_REQUEST_MID_H
#define PACKET_GM_REQUEST_MID_H

#include "PacketHeader.h"

class Packet_GM_Request_Mid : public PacketHeader {
public:
    Packet_GM_Request_Mid() : PacketHeader(0x76c, 0x34){};
};

#endif  // PACKET_GM_REQUEST_MID_H
