//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PVPCHANNELINFO_H
#define PACKET_PVPCHANNELINFO_H

#include "PacketHeader.h"

class Packet_PvPChannelInfo : public PacketHeader {
public:
    Packet_PvPChannelInfo() : PacketHeader(0xfa2, 0x17f8){};
};

#endif  // PACKET_PVPCHANNELINFO_H
