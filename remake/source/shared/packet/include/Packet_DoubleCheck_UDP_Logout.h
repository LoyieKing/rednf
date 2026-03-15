//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DOUBLECHECK_UDP_LOGOUT_H
#define PACKET_DOUBLECHECK_UDP_LOGOUT_H

#include "PacketHeader.h"

class Packet_DoubleCheck_UDP_Logout : public PacketHeader {
public:
    Packet_DoubleCheck_UDP_Logout() : PacketHeader(0xbb9, 0x10){};
};

#endif  // PACKET_DOUBLECHECK_UDP_LOGOUT_H
