//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_LOGIN_H
#define PACKET_HADES_LOGIN_H

#include "PacketHeader.h"

class Packet_Hades_Login : public PacketHeader {
public:
    Packet_Hades_Login() : PacketHeader(0xfd2, 0x64){};
};

#endif  // PACKET_HADES_LOGIN_H
