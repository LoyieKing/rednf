//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PARTY_RESULT_STATISTIC_H
#define PACKET_PARTY_RESULT_STATISTIC_H

#include "PacketHeader.h"

class Packet_Party_Result_Statistic : public PacketHeader {
public:
    Packet_Party_Result_Statistic() : PacketHeader(0xfa6, 0xe){};
};

#endif  // PACKET_PARTY_RESULT_STATISTIC_H
