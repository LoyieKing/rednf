//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_SECRET_SHOP_STATISTIC_H
#define PACKET_SECRET_SHOP_STATISTIC_H

#include "PacketHeader.h"

class Packet_Secret_Shop_Statistic : public PacketHeader {
public:
    int m_fieldA;  // +a
    char m_pade[0xfa4];  // +e..+fb1（ctor 未触碰/布局待定）

    Packet_Secret_Shop_Statistic() : PacketHeader(0x1b69, 0xfb2),
        m_fieldA(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Secret_Shop_Statistic, 0xfb2);

#endif  // PACKET_SECRET_SHOP_STATISTIC_H
