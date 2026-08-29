//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_BROADCAST_MSG_H
#define PACKET_BROADCAST_MSG_H

#include "PacketHeader.h"

class Packet_Broadcast_Msg : public PacketHeader {
public:
    int m_padA;              // +0xa（ORIG _BroadcastRunaway@82eab7a 整字置 0）
    unsigned char m_len;     // +0xe（_BroadcastRunaway：packetSize = len + 0xf）
    char m_text[0xff];       // +0xf（_BroadcastRunaway：strncpy 0xff）
    Packet_Broadcast_Msg() : PacketHeader(0x2711, 0x10e){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Broadcast_Msg, 0x10e);

#endif  // PACKET_BROADCAST_MSG_H
