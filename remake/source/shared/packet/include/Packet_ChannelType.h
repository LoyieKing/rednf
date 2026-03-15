//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CHANNELTYPE_H
#define PACKET_CHANNELTYPE_H

#include "PacketHeader.h"

class Packet_ChannelType : public PacketHeader {
public:
    Packet_ChannelType() : PacketHeader(0xfa4, 0x12){};
};

#endif  // PACKET_CHANNELTYPE_H
