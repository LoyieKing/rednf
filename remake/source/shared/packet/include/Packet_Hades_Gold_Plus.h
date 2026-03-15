//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_GOLD_PLUS_H
#define PACKET_HADES_GOLD_PLUS_H

#include "PacketHeader.h"

class Packet_Hades_Gold_Plus : public PacketHeader {
public:
    Packet_Hades_Gold_Plus() : PacketHeader(0xfda, 0x14){};
};

#endif  // PACKET_HADES_GOLD_PLUS_H
