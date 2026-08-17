//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_SYNC_GM_LIST_H
#define PACKET_SYNC_GM_LIST_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Sync_GM_List : public PacketHeader {
public:
    unsigned char m_pageNo;       // +0xa（分包序号；SendGMList_To_UpperServer 每发满 0x14 成员 +1）
    unsigned char m_gmCount;      // +0xb（本包 GM 数；满批 0x14，末批为余数）
    unsigned int m_gmIds[0x14];   // +0xc（auStack_84[20]，每成员一个 GM id）
    char m_gmNames[0x14];         // +0x5c（auStack_34[20]，每 GM 1 字节；包大小 0x70 修正）
    Packet_Sync_GM_List() : PacketHeader(0x1f4d, 0x70),
        m_pageNo(0), m_gmCount(0)
    {
    memset(m_gmIds, 0, sizeof(m_gmIds));
    memset((char*)this + 0x5c, 0, 0x14);  // m_gmNames
    };
} __attribute__((packed));

#endif  // PACKET_SYNC_GM_LIST_H
