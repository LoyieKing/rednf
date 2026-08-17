//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_FAIR_PVP_PING_STATISTIC_H
#define PACKET_FAIR_PVP_PING_STATISTIC_H

#include "PacketHeader.h"

class Packet_Fair_Pvp_Ping_Statistic : public PacketHeader {
public:
    unsigned short m_badPing;  // +a（fairPvpPingStat *param_2；statics PushFairPvpPingData 累加到
                               //      m_fairPvpBadPing / DB fair_pvp_bad_ping）
    short m_total;             // +c（*param_3 + *param_2；statics 累加到 m_fairPvpTotal / DB fair_pvp_total）

    Packet_Fair_Pvp_Ping_Statistic() : PacketHeader(0x27ed, 0xe),
        m_badPing(0), m_total(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Fair_Pvp_Ping_Statistic, 0xe);

#endif  // PACKET_FAIR_PVP_PING_STATISTIC_H
