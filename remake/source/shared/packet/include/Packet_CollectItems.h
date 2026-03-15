//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_COLLECTITEMS_H
#define PACKET_COLLECTITEMS_H

#include "PacketHeader.h"

class Packet_CollectItems : public PacketHeader {
public:
    Packet_CollectItems() : PacketHeader(0x27e6, 0x39){};
};

#endif  // PACKET_COLLECTITEMS_H
