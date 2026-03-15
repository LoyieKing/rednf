//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DELETE_TO_BLACKLIST_H
#define PACKET_DELETE_TO_BLACKLIST_H

#include "PacketHeader.h"

class Packet_Delete_To_BlackList : public PacketHeader {
public:
    Packet_Delete_To_BlackList() : PacketHeader(0x5dd, 0x2c){};
};

#endif  // PACKET_DELETE_TO_BLACKLIST_H
