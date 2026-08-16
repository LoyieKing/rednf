//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MAX_LEVEL_BROADCAST_H
#define PACKET_MONITOR_MAX_LEVEL_BROADCAST_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Max_Level_BroadCast : public PacketHeader {
public:
    char m_fielda[0x1e];  // +a..+27（ctor memset 区域）

    Packet_Monitor_Max_Level_BroadCast() : PacketHeader(0x177e, 0x28)
    {
    memset(m_fielda, 0, sizeof(m_fielda));
    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Monitor_Max_Level_BroadCast, 0x28);


#endif //PACKET_MONITOR_MAX_LEVEL_BROADCAST_H
