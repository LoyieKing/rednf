//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_P2P_STATISTICS_H
#define PACKET_P2P_STATISTICS_H

#include "PacketHeader.h"

class Packet_P2P_Statistics : public PacketHeader {
public:
    Packet_P2P_Statistics() : PacketHeader(0x27e1, 0x3f){};
};

#endif  // PACKET_P2P_STATISTICS_H
