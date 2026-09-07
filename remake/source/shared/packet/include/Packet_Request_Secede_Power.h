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
    unsigned char m_field12;    // +0x12（ORIG packetSize 0x13 实证 @0x846f812 C1；
                                //   C1/发送端未消费，占位保 sizeof 0x13）
    Packet_Request_Secede_Power() : PacketHeader(0x6a9, 0x13){};
} __attribute__((packed));

#endif  // PACKET_REQUEST_SECEDE_POWER_H
