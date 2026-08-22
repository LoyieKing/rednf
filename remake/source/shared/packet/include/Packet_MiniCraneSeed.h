//
// df_game_r 还原 —— Packet_MiniCraneSeed（typo 批次）。
// ORIG ctor 0x086d1d18（W）：PacketHeader(0x27f8, 0xe) + this+0x0a 4 字节清零。
// 发送方 GameWorld::UpdateMiniCraneSeed @ 0x086d1480，包长 0xe。
//

#ifndef PACKET_MINICRANESEED_H
#define PACKET_MINICRANESEED_H

#include "PacketHeader.h"

class Packet_MiniCraneSeed : public PacketHeader {
public:
    unsigned int m_data;  // +0x0a（ctor 清零）
    Packet_MiniCraneSeed() : PacketHeader(0x27f8, 0xe), m_data(0)
    {
    }
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_MiniCraneSeed, 0xe);

#endif  // PACKET_MINICRANESEED_H
