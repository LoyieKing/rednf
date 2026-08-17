//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_ABNORMAL_EXIT_STATISTIC_H
#define PACKET_ABNORMAL_EXIT_STATISTIC_H

#include "PacketHeader.h"

class Packet_Abnormal_Exit_Statistic : public PacketHeader {
public:
    unsigned short m_successDungeonClear; // +0xa（dispatch_sig 首个 get_short；UdpCharacteristic::PushAbnormalExitData fieldA→m_successDungeonClear）
    unsigned short m_totalDungeonClear;   // +0xc（第二个 get_short + 首值；fieldB→m_totalDungeonClear）

    Packet_Abnormal_Exit_Statistic() : PacketHeader(0xfa9, 0xe){};
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Abnormal_Exit_Statistic, 0xe);

#endif  // PACKET_ABNORMAL_EXIT_STATISTIC_H
