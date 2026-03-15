//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_ABNORMAL_EXIT_STATISTIC_H
#define PACKET_ABNORMAL_EXIT_STATISTIC_H

#include "PacketHeader.h"

class Packet_Abnormal_Exit_Statistic : public PacketHeader {
public:
    Packet_Abnormal_Exit_Statistic() : PacketHeader(0xfa9, 0xe){};
};

#endif  // PACKET_ABNORMAL_EXIT_STATISTIC_H
