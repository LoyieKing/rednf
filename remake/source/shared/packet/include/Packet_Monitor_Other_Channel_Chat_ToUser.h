//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PACKET_MONITOR_OTHER_CHANNEL_CHAT_TOUSER_H
#define REDNF_PACKET_MONITOR_OTHER_CHANNEL_CHAT_TOUSER_H

#include "PacketHeader.h"
#include <cstring>

#pragma pack(push, 1)
class Packet_Monitor_Other_Channel_Chat_ToUser : public PacketHeader {
public:
    int what_0x0a;                  // offset 0x0a
    char server_id;                 // offset 0x0e
    int sTGameUserInfo_what3_0x05;  // offset 0x0f
    int charac_no;                  // offset 0x13
    char buddy_n_user_id_what[30];  // offset 0x17
    char errorCode;                 // offset 0x35
    unsigned char chatLength;       // offset 0x36（原始：movzx 零扩展）
    char chatContent[0x100];        // offset 0x37

    Packet_Monitor_Other_Channel_Chat_ToUser() : PacketHeader(0x3f3, 0x137) {
        // 初始化行为对齐原始二进制（2026-08-06 反汇编验证）
        server_id = 0;
        sTGameUserInfo_what3_0x05 = -1;
        charac_no = 0;
        errorCode = 0;
        chatLength = 0;
        memset(buddy_n_user_id_what, 0, sizeof(buddy_n_user_id_what));
        memset(chatContent, 0, sizeof(chatContent));
    }
} __attribute__((packed));
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Monitor_Other_Channel_Chat_ToUser, 0x137);

#endif  // REDNF_PACKET_MONITOR_OTHER_CHANNEL_CHAT_TOUSER_H
