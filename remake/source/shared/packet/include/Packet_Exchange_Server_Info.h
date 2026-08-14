//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_EXCHANGE_SERVER_INFO_H
#define PACKET_EXCHANGE_SERVER_INFO_H

#include "PacketHeader.h"

class Packet_Exchange_Server_Info : public PacketHeader {
public:
    unsigned int m_channel;         // +0xa  also used as signed code
    short m_port;                   // +0xe
    unsigned int m_ip;              // +0x10
    Packet_Exchange_Server_Info() : PacketHeader(0x9de, 0x14){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Exchange_Server_Info, 0x14);

#endif  // PACKET_EXCHANGE_SERVER_INFO_H
