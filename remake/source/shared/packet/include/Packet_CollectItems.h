//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_COLLECTITEMS_H
#define PACKET_COLLECTITEMS_H

#include "PacketHeader.h"

class Packet_CollectItems : public PacketHeader {
public:
    unsigned int m_idByChannel;  // +0xa
    unsigned int m_charNo;       // +0xe
    int m_add;                   // +0x12
    unsigned int m_pad16;        // +0x16
    unsigned char m_nameLen;     // +0x1a
    char m_name[0x1e];           // +0x1b
    Packet_CollectItems() : PacketHeader(0x27e6, 0x39){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_CollectItems, 0x39);

#endif  // PACKET_COLLECTITEMS_H
