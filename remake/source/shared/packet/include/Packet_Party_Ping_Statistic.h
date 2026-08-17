//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PARTY_PING_STATISTIC_H
#define PACKET_PARTY_PING_STATISTIC_H

#include "PacketHeader.h"

class Packet_Party_Ping_Statistic : public PacketHeader {
public:
    unsigned short m_dungeonBadPing; // +0xa（partyPingStat *param_2；UdpCharacteristic::PushPartyPingData fieldA→m_dungeonBadPing）
    unsigned short m_dungeonTotal;   // +0xc（*param_2+*param_3；fieldB→m_dungeonTotal）

    Packet_Party_Ping_Statistic() : PacketHeader(0xfa7, 0xe){};
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Party_Ping_Statistic, 0xe);

#endif  // PACKET_PARTY_PING_STATISTIC_H
