//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_FATIGUE_BATTERY_MONEY_STATISTIC_H
#define PACKET_FATIGUE_BATTERY_MONEY_STATISTIC_H

#include "PacketHeader.h"

class Packet_Fatigue_Battery_Money_Statistic : public PacketHeader {
public:
    char m_fieldA;  // +a
    int m_fieldB;  // +b
    short m_fieldC;  // +f

    Packet_Fatigue_Battery_Money_Statistic() : PacketHeader(0xc4a, 0x11),
        m_fieldA(0), m_fieldB(0), m_fieldC(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Fatigue_Battery_Money_Statistic, 0x11);


#endif //PACKET_FATIGUE_BATTERY_MONEY_STATISTIC_H
