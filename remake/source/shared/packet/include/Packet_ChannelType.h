//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CHANNELTYPE_H
#define PACKET_CHANNELTYPE_H

#include "PacketHeader.h"

class Packet_ChannelType : public PacketHeader {
public:
    unsigned int m_channel;         // +0xa  FindGameServer
    int m_type;                     // +0xe  SetChannelType
    Packet_ChannelType() : PacketHeader(0xfa4, 0x12){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_ChannelType, 0x12);

#endif  // PACKET_CHANNELTYPE_H
