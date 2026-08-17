//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PARTY_RESULT_STATISTIC_H
#define PACKET_PARTY_RESULT_STATISTIC_H

#include "PacketHeader.h"

class Packet_Party_Result_Statistic : public PacketHeader {
public:
    unsigned short m_successPartyTry; // +0xa（dispatch_sig 首个 get_short；UdpCharacteristic::PushPartyResultData fieldA→m_successPartyTry）
    unsigned short m_totalPartyTry;   // +0xc（第二个 get_short + 首值；fieldB→m_totalPartyTry）

    Packet_Party_Result_Statistic() : PacketHeader(0xfa6, 0xe){};
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Party_Result_Statistic, 0xe);

#endif  // PACKET_PARTY_RESULT_STATISTIC_H
