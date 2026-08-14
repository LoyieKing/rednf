//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_SECEDE_POWER_H
#define PACKET_REQUEST_SECEDE_POWER_H

#include "PacketHeader.h"

class Packet_Request_Secede_Power : public PacketHeader {
public:
    unsigned int m_charNo;      // +0xa
    unsigned int m_guildKey;    // +0xe
    Packet_Request_Secede_Power() : PacketHeader(0x6a9, 0x13){};
} __attribute__((packed));

#endif  // PACKET_REQUEST_SECEDE_POWER_H
