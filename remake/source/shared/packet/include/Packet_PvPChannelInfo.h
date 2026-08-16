//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PVPCHANNELINFO_H
#define PACKET_PVPCHANNELINFO_H

#include "PacketHeader.h"

class Packet_PvPChannelInfo : public PacketHeader {
public:
    unsigned int m_charNo;          // +0xa  FindUser_CharNo
    unsigned int m_uid;          // +0xe
    unsigned int m_field12;         // +0x12
    unsigned char m_channelCount;   // +0x16  inbound request
    unsigned char m_count;          // +0x17  reply
    char m_channels[0x17e0];        // +0x18  STPvPChannelInfo[]
    Packet_PvPChannelInfo() : PacketHeader(0xfa2, 0x17f8){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_PvPChannelInfo, 0x17f8);

#endif  // PACKET_PVPCHANNELINFO_H
