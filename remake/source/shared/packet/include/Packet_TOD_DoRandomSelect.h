//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_TOD_DORANDOMSELECT_H
#define PACKET_TOD_DORANDOMSELECT_H

#include "PacketHeader.h"

class Packet_TOD_DoRandomSelect : public PacketHeader {
public:
    Packet_TOD_DoRandomSelect() : PacketHeader(0x2712, 0xa){};
};

#endif  // PACKET_TOD_DORANDOMSELECT_H
