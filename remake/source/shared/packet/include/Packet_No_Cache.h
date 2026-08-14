//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_NO_CACHE_H
#define PACKET_NO_CACHE_H

#include "PacketHeader.h"

class Packet_No_Cache : public PacketHeader {
public:
    unsigned int m_dbid;        // +0xa
    unsigned int m_fieldE;      // +0xe
    unsigned int m_field12;     // +0x12
    Packet_No_Cache() : PacketHeader(0x1b6d, 0x16){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_No_Cache, 0x16);

#endif  // PACKET_NO_CACHE_H
