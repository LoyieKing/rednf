//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_COLLECTITEMSGM_H
#define PACKET_COLLECTITEMSGM_H

#include "PacketHeader.h"

class Packet_CollectItemsGm : public PacketHeader {
public:
    Packet_CollectItemsGm() : PacketHeader(0x27eb, 0x17){};
} __attribute__((packed));

#endif  // PACKET_COLLECTITEMSGM_H
