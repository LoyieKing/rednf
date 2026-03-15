//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DOUBLECHECK_UDP_LOGIN_H
#define PACKET_DOUBLECHECK_UDP_LOGIN_H

#include "PacketHeader.h"

class Packet_DoubleCheck_UDP_Login : public PacketHeader {
public:
    Packet_DoubleCheck_UDP_Login() : PacketHeader(0xbb8, 0x10){};
};

#endif  // PACKET_DOUBLECHECK_UDP_LOGIN_H
