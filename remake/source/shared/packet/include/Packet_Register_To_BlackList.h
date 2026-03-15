//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REGISTER_TO_BLACKLIST_H
#define PACKET_REGISTER_TO_BLACKLIST_H

#include "PacketHeader.h"

class Packet_Register_To_BlackList : public PacketHeader {
public:
    Packet_Register_To_BlackList() : PacketHeader(0x5dc, 0x2c){};
};

#endif  // PACKET_REGISTER_TO_BLACKLIST_H
