//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_JOIN_POWER_H
#define PACKET_REQUEST_JOIN_POWER_H

#include "PacketHeader.h"

class Packet_Request_Join_Power : public PacketHeader {
public:
    unsigned int m_charNo;      // +0xa
    unsigned int m_guildKey;    // +0xe
    unsigned char m_field12;    // +0x12
    Packet_Request_Join_Power() : PacketHeader(0x6a8, 0x13){};
} __attribute__((packed));

#endif  // PACKET_REQUEST_JOIN_POWER_H
