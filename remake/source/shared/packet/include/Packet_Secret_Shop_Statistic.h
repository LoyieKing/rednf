//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_SECRET_SHOP_STATISTIC_H
#define PACKET_SECRET_SHOP_STATISTIC_H

#include "PacketHeader.h"

class Packet_Secret_Shop_Statistic : public PacketHeader {
public:
    Packet_Secret_Shop_Statistic() : PacketHeader(0x1b69, 0xfb2){};
};

#endif  // PACKET_SECRET_SHOP_STATISTIC_H
