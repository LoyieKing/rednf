//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PVPRESULT_H
#define PACKET_PVPRESULT_H

#include "PacketHeader.h"

class Packet_PvPResult : public PacketHeader {
public:
    Packet_PvPResult() : PacketHeader(0xfa5, 0x80){};
    char m_pad[0x80 - 0xa];   // 结果包体 0x76B（CLeagueMatch 依裸偏移写入 eloK2@+0xa/winType@+0xe/flag 区）
} __attribute__((packed));

#endif  // PACKET_PVPRESULT_H
