//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PVP_PING_STATISTIC_H
#define PACKET_PVP_PING_STATISTIC_H

#include "PacketHeader.h"

class Packet_Pvp_Ping_Statistic : public PacketHeader {
public:
    unsigned short m_pvpBadPing; // +0xa（pvpPingStat *param_2；UdpCharacteristic::PushPvpPingData fieldA→m_pvpBadPing）
    unsigned short m_pvpTotal;   // +0xc（*param_3+*param_2；fieldB→m_pvpTotal）

    Packet_Pvp_Ping_Statistic() : PacketHeader(0xfa8, 0xe){};
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Pvp_Ping_Statistic, 0xe);

#endif  // PACKET_PVP_PING_STATISTIC_H
