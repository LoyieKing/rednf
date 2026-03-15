//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_VILLAGEMONSTERFIGHTRESULT_H
#define PACKET_VILLAGEMONSTERFIGHTRESULT_H

#include "PacketHeader.h"

class Packet_VillageMonsterFightResult : public PacketHeader {
public:
    Packet_VillageMonsterFightResult(): PacketHeader(0x1772,0x2b) {};
};


#endif //PACKET_VILLAGEMONSTERFIGHTRESULT_H
