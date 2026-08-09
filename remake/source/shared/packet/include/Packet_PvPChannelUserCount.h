//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PVPCHANNELUSERCOUNT_H
#define PACKET_PVPCHANNELUSERCOUNT_H

#include "PacketHeader.h"

class Packet_PvPChannelUserCount : public PacketHeader {
public:
    Packet_PvPChannelUserCount() : PacketHeader(0xfa3, 0x2a){};
} __attribute__((packed));

#endif  // PACKET_PVPCHANNELUSERCOUNT_H
