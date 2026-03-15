//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PVPRESULT_H
#define PACKET_PVPRESULT_H

#include "PacketHeader.h"

class Packet_PvPResult : public PacketHeader {
public:
    Packet_PvPResult() : PacketHeader(0xfa5, 0x80){};
};

#endif  // PACKET_PVPRESULT_H
