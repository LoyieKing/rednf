//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PCROOMPLAYTIMEREWARD_H
#define PACKET_PCROOMPLAYTIMEREWARD_H

#include "PacketHeader.h"

class Packet_PcRoomPlayTimeReward : public PacketHeader {
public:
    Packet_PcRoomPlayTimeReward() : PacketHeader(0x27ec, 0x13){};
};

#endif  // PACKET_PCROOMPLAYTIMEREWARD_H
