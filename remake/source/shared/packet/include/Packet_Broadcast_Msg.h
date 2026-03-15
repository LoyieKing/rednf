//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_BROADCAST_MSG_H
#define PACKET_BROADCAST_MSG_H

#include "PacketHeader.h"

class Packet_Broadcast_Msg : public PacketHeader {
public:
    Packet_Broadcast_Msg() : PacketHeader(0x2711, 0x10e){};
};

#endif  // PACKET_BROADCAST_MSG_H
