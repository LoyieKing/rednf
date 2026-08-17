//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_FATIGUE_BATTERY_MONEY_STATISTIC_H
#define PACKET_FATIGUE_BATTERY_MONEY_STATISTIC_H

#include "PacketHeader.h"

class Packet_Fatigue_Battery_Money_Statistic : public PacketHeader {
public:
    unsigned char m_serverGroup;  // +a（ActiveFatigueBattery_NotEndLevel: CEnvironment::get_server_group；DB log_fatigue_battery.server_id）
    int m_money;                  // +b（DB log_fatigue_battery.money；SendDBFatigueBattery STFatigueBattery::m_money）
    unsigned short m_buff;        // +f（DB log_fatigue_battery.buff；SendDBFatigueBattery STFatigueBattery::m_buff）

    Packet_Fatigue_Battery_Money_Statistic() : PacketHeader(0xc4a, 0x11),
        m_serverGroup(0), m_money(0), m_buff(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Fatigue_Battery_Money_Statistic, 0x11);


#endif //PACKET_FATIGUE_BATTERY_MONEY_STATISTIC_H
