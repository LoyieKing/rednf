//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_GOLDTRACKING_H
#define PACKET_HADES_GOLDTRACKING_H

#include "PacketHeader.h"

class Packet_Hades_GoldTracking : public PacketHeader {
public:
    Packet_Hades_GoldTracking() : PacketHeader(0xfdc, 0x29){};
};

#endif  // PACKET_HADES_GOLDTRACKING_H
