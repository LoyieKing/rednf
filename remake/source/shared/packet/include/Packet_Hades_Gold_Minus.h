//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_GOLD_MINUS_H
#define PACKET_HADES_GOLD_MINUS_H

#include "PacketHeader.h"

class Packet_Hades_Gold_Minus : public PacketHeader {
public:
    Packet_Hades_Gold_Minus() : PacketHeader(0xfdb, 0x14){};
};

#endif  // PACKET_HADES_GOLD_MINUS_H
