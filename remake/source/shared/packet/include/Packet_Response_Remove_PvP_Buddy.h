//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PACKET_RESPONSE_REMOVE_PVP_BUDDY_H
#define REDNF_PACKET_RESPONSE_REMOVE_PVP_BUDDY_H

#include "PacketHeader.h"
#include <cstring>

#pragma pack(push, 1)
class Packet_Response_Remove_PvP_Buddy : public PacketHeader {
public:
    int charac_no;                          // offset 10
    int m_uid;                              // offset 14（notice_remove_buddy_fail/success：
                                            //          = stGameUserInfo.m_uid（STGameUserInfo +0x05））
    char server_id;               // offset 0x12
    char m_name[0x1e];                      // offset 0x13（notice_remove_buddy_success：
                                            //          = 被删好友角色名（req_remove_buddy 参数），
                                            //          memcpy 0x1d）
    unsigned char m_errorCode;              // offset 0x31（notice_remove_buddy_fail 的 error_code；
                                            //          0=成功，3=好友不存在）

    Packet_Response_Remove_PvP_Buddy() : PacketHeader(0x1b61, 0x32) {
        // 初始化行为对齐原始二进制（2026-08-06 反汇编验证）
        charac_no = 0;
        m_uid = 0;
        server_id = 0;
        m_errorCode = 0;
        memset(m_name, 0, sizeof(m_name));
    }
} __attribute__((packed));
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Response_Remove_PvP_Buddy, 0x32);

#endif  // REDNF_PACKET_RESPONSE_REMOVE_PVP_BUDDY_H
