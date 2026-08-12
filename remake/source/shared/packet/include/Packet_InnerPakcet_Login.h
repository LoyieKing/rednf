//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_INNERPAKCET_LOGIN_H
#define PACKET_INNERPAKCET_LOGIN_H

#include "PacketHeader.h"

class Packet_InnerPakcet_Login : public PacketHeader {
public:
    Packet_InnerPakcet_Login();
} __attribute__((packed));

#endif  // PACKET_INNERPAKCET_LOGIN_H
