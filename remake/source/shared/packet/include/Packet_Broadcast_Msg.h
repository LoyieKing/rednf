//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_BROADCAST_MSG_H
#define PACKET_BROADCAST_MSG_H

#include "PacketHeader.h"

class Packet_Broadcast_Msg : public PacketHeader {
public:
    char m_padA[4];          // +0xa
    unsigned char m_len;     // +0xe
    char m_text[0xff];       // +0xf
    Packet_Broadcast_Msg() : PacketHeader(0x2711, 0x10e){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Broadcast_Msg, 0x10e);

#endif  // PACKET_BROADCAST_MSG_H
