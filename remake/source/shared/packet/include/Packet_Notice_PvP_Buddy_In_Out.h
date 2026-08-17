//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PACKET_NOTICE_PVP_BUDDY_IN_OUT_H
#define REDNF_PACKET_NOTICE_PVP_BUDDY_IN_OUT_H

#include "PacketHeader.h"
#include <cstring>

#pragma pack(push, 1)
class Packet_Notice_PvP_Buddy_In_Out : public PacketHeader {
public:
    int m_characNo;      // offset 0xa（Community CUser::notice_login_logout：接收方 buddy 的 stGameUserInfo.charac_no）
    int m_uid;           // offset 0xe（notice_login_logout：= 接收方 buddy 的 stGameUserInfo.m_uid
                         //          （STGameUserInfo offset 0x05），与同族包的 m_uid 字段一致）
    char m_loginout;     // offset 0x12（notice_login_logout：CUser::eLoginout，0=登录 1=登出）
    char channel_no;      // offset 0x13
    char server_id;      // offset 0x14
    char m_name[30];     // offset 0x15（notice_login_logout：登入/登出者 stGameUserInfo.m_name，
                         //          memcpy 0x1d）

    Packet_Notice_PvP_Buddy_In_Out() : PacketHeader(0x1b62, 0x33) {
        // 初始化行为对齐原始二进制（2026-08-06 反汇编验证）
        m_characNo = 0;
        m_uid = 0;
        m_loginout = 0;
        channel_no = 0;
        server_id = 0;
        memset(m_name, 0, sizeof(m_name));
    }
} __attribute__((packed));
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Notice_PvP_Buddy_In_Out, 0x33);

#endif  // REDNF_PACKET_NOTICE_PVP_BUDDY_IN_OUT_H
