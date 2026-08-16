//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CLIENT_SPEC_STATISTIC_H
#define PACKET_CLIENT_SPEC_STATISTIC_H

#include "PacketHeader.h"

class Packet_Client_Spec_Statistic : public PacketHeader {
public:
    char m_pada[0x1];  // +a..+a（ctor 未触碰/布局待定）
    char m_fieldA;  // +b
    char m_padc[0xbfa];  // +c..+c05（ctor 未触碰/布局待定）

    Packet_Client_Spec_Statistic() : PacketHeader(0x9ce, 0xc06),
        m_fieldA(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Client_Spec_Statistic, 0xc06);

#endif  // PACKET_CLIENT_SPEC_STATISTIC_H
