//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_BLOOD_DUNGEON_STATISTIC_H
#define PACKET_BLOOD_DUNGEON_STATISTIC_H

#include "PacketHeader.h"

class Packet_Blood_dungeon_statistic : public PacketHeader {
public:
    char m_padA[4];         // +0xa..+0xd（发送端不写、接收端不读，栈残留；接收端 Wire m_hdr[0xe] 覆盖）
    unsigned int m_level;       // +0xe（OnDungeonClear get_charac_level；接收端 m_blood key；DB log_blood_dungeon.level）
    unsigned char m_tryCount;   // +0x12（发送端写 1；接收端 m_f0→m_tryCount；DB try_count）
    unsigned char m_clearCount; // +0x13（发送端 param_2 通关标志；接收端 m_f1→m_clearCount；DB clear_count）

    Packet_Blood_dungeon_statistic() : PacketHeader(0x1036, 0x14){};
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Blood_dungeon_statistic, 0x14);

#endif  // PACKET_BLOOD_DUNGEON_STATISTIC_H
