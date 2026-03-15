//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CLIENT_SPEC_STATISTIC_H
#define PACKET_CLIENT_SPEC_STATISTIC_H

#include "PacketHeader.h"

class Packet_Client_Spec_Statistic : public PacketHeader {
public:
    Packet_Client_Spec_Statistic() : PacketHeader(0x9ce, 0xc06){};
};

#endif  // PACKET_CLIENT_SPEC_STATISTIC_H
