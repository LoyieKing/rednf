//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PACKET_UPDATE_PVP_BUDDY_H
#define REDNF_PACKET_UPDATE_PVP_BUDDY_H

#include "PacketHeader.h"
#include <cstring>

#pragma pack(push, 1)
class Packet_Update_PvP_Buddy : public PacketHeader {
public:
    char server_id;       // offset 10
    int charac_no;        // offset 0xb
    char m_level;         // offset 0xf（Community db_update_buddy：= stGameUserInfo.m_level）
    char m_growType;      // offset 0x10（Community db_update_buddy：= stGameUserInfo.m_growType）

    Packet_Update_PvP_Buddy() : PacketHeader(0x1b65, 0x11) {
        // 初始化行为对齐原始二进制（2026-08-06 反汇编验证）
        server_id = 0;
        charac_no = 0;
        m_level = 0;
        m_growType = 0;
    }
} __attribute__((packed));
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Update_PvP_Buddy, 0x11);

#endif  // REDNF_PACKET_UPDATE_PVP_BUDDY_H
