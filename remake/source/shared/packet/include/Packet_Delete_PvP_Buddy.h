//
// Created by loyieking on 24-3-24.
//

#ifndef REDNF_PACKET_DELETE_PVP_BUDDY_H
#define REDNF_PACKET_DELETE_PVP_BUDDY_H

#include "PacketHeader.h"
#include <cstring>

class Packet_Delete_PvP_Buddy : public PacketHeader {
public:
    char user_server_id;
    uint user_charac_no;
    char buddy_server_id;
    uint buddy_charac_no;

    Packet_Delete_PvP_Buddy() : PacketHeader(0x1b64, 0x14) {
        // 初始化行为对齐原始二进制（2026-08-06 反汇编验证）
        user_server_id = 0;
        user_charac_no = 0;
        buddy_server_id = 0;
        buddy_charac_no = 0;
} __attribute__((packed));
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Delete_PvP_Buddy, 0x14);

#endif  // REDNF_PACKET_DELETE_PVP_BUDDY_H
