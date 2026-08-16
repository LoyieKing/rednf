//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REGISTER_TO_BLACKLIST_H
#define PACKET_REGISTER_TO_BLACKLIST_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Register_To_BlackList : public PacketHeader {
public:
    unsigned int m_dbid;            // +0xa  FindUser
    char m_name[0x1e];              // +0xe
    Packet_Register_To_BlackList() : PacketHeader(0x5dc, 0x2c)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Register_To_BlackList, 0x2c);

#endif  // PACKET_REGISTER_TO_BLACKLIST_H
