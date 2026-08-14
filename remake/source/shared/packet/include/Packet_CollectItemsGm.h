//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_COLLECTITEMSGM_H
#define PACKET_COLLECTITEMSGM_H

#include "PacketHeader.h"

class Packet_CollectItemsGm : public PacketHeader {
public:
    unsigned char m_serverGroup;  // +0xa
    char m_pad[0xc];              // +0xb
    Packet_CollectItemsGm() : PacketHeader(0x27eb, 0x17){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_CollectItemsGm, 0x17);

#endif  // PACKET_COLLECTITEMSGM_H
