//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_FILE_STATISTIC_H
#define PACKET_FILE_STATISTIC_H

#include "PacketHeader.h"
#include <string.h>

class Packet_File_Statistic : public PacketHeader {
public:
    char m_fielda[0x100];  // +a..+109（ctor memset 区域）
    char m_field10a[0x800];  // +10a..+909（ctor memset 区域）

    Packet_File_Statistic() : PacketHeader(0x27f3, 0x90a)
    {
    memset(m_fielda, 0, sizeof(m_fielda));
    memset(m_field10a, 0, sizeof(m_field10a));
    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_File_Statistic, 0x90a);

#endif  // PACKET_FILE_STATISTIC_H
