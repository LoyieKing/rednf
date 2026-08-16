//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_SYNC_GM_LIST_H
#define PACKET_SYNC_GM_LIST_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Sync_GM_List : public PacketHeader {
public:
    unsigned char m_field_a;      // +0xa
    unsigned char m_field_b;      // +0xb
    unsigned int m_gmIds[0x14];   // +0xc
    char m_gmNames[0x100];        // +0x5c
    Packet_Sync_GM_List() : PacketHeader(0x1f4d, 0x70),
        m_field_a(0), m_field_b(0)
    {
    memset(m_gmIds, 0, sizeof(m_gmIds));
    memset((char*)this + 0x5c, 0, 0x14);  // m_gmNames
    };
} __attribute__((packed));

#endif  // PACKET_SYNC_GM_LIST_H
