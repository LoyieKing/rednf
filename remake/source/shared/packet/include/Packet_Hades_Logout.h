//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_LOGOUT_H
#define PACKET_HADES_LOGOUT_H

#include "PacketHeader.h"

class Packet_Hades_Logout : public PacketHeader {
public:
    Packet_Hades_Logout() : PacketHeader(0xfd3, 0xe){};
};

#endif  // PACKET_HADES_LOGOUT_H
