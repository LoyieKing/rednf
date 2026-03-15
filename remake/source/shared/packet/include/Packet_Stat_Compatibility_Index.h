//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_STAT_COMPATIBILITY_INDEX_H
#define PACKET_STAT_COMPATIBILITY_INDEX_H

#include "PacketHeader.h"

class Packet_Stat_Compatibility_Index : public PacketHeader {
public:
    Packet_Stat_Compatibility_Index(): PacketHeader(0x27e0,0x1e) {};
};


#endif //PACKET_STAT_COMPATIBILITY_INDEX_H
