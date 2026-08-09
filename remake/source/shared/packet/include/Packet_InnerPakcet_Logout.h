//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_INNERPAKCET_LOGOUT_H
#define PACKET_INNERPAKCET_LOGOUT_H

#include "PacketHeader.h"

class Packet_InnerPakcet_Logout : public PacketHeader {
public:
    Packet_InnerPakcet_Logout() : PacketHeader(0xfa1, 0xa){};
} __attribute__((packed));

#endif  // PACKET_INNERPAKCET_LOGOUT_H
