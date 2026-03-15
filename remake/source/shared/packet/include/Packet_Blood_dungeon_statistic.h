//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_BLOOD_DUNGEON_STATISTIC_H
#define PACKET_BLOOD_DUNGEON_STATISTIC_H

#include "PacketHeader.h"

class Packet_Blood_dungeon_statistic : public PacketHeader {
public:
    Packet_Blood_dungeon_statistic() : PacketHeader(0x1036, 0x14){};
};

#endif  // PACKET_BLOOD_DUNGEON_STATISTIC_H
