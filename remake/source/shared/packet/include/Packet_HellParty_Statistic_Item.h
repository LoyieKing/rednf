//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HELLPARTY_STATISTIC_ITEM_H
#define PACKET_HELLPARTY_STATISTIC_ITEM_H

#include "PacketHeader.h"

class Packet_HellParty_Statistic_Item : public PacketHeader {
public:
    Packet_HellParty_Statistic_Item() : PacketHeader(0xc31, 0x2a){};
};

#endif  // PACKET_HELLPARTY_STATISTIC_ITEM_H
