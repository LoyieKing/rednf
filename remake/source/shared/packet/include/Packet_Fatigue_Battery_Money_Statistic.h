//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_FATIGUE_BATTERY_MONEY_STATISTIC_H
#define PACKET_FATIGUE_BATTERY_MONEY_STATISTIC_H

#include "PacketHeader.h"

class Packet_Fatigue_Battery_Money_Statistic : public PacketHeader {
public:
    Packet_Fatigue_Battery_Money_Statistic(): PacketHeader(0xc4a,0x11) {};
};


#endif //PACKET_FATIGUE_BATTERY_MONEY_STATISTIC_H
