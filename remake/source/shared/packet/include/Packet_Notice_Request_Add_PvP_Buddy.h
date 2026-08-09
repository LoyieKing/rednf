//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PACKET_NOTICE_REQUEST_ADD_PVP_BUDDY_H
#define REDNF_PACKET_NOTICE_REQUEST_ADD_PVP_BUDDY_H

#include "PacketHeader.h"
#include <cstring>

#pragma pack(push, 1)
class Packet_Notice_Request_Add_PvP_Buddy : public PacketHeader {
public:
    int charac_no;                    // offset 10
    int sTGameUserInfo_what3_0x05;    // offset 14
    char server_id;                   // offset 0x12
    char buddy_n_user_id_what[0x1e];  // offset 0x13

    Packet_Notice_Request_Add_PvP_Buddy()
        : PacketHeader(0x1b5d, 0x31) {
        // 初始化行为对齐原始二进制（2026-08-06 反汇编验证）
        charac_no = 0;
        sTGameUserInfo_what3_0x05 = 0;
        server_id = 0;
        memset(buddy_n_user_id_what, 0, sizeof(buddy_n_user_id_what));
    }
} __attribute__((packed));
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Notice_Request_Add_PvP_Buddy, 0x31);

#endif  // REDNF_PACKET_NOTICE_REQUEST_ADD_PVP_BUDDY_H
