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
    int m_senderCharId;             // offset 0x0a（monitor ORIG：reply.m_senderCharId = chat->what_0x0a）
    char m_serverId;                // offset 0x0e（monitor ORIG ctor：m_serverId = 0）
    int m_idByChannel;              // offset 0x0f（GetIdByChannel；ctor 0xffffffff）
    int m_uniqCharNo;               // offset 0x13（GetUniqCharNo）
    char m_name[0x1e];              // offset 0x17（memcpy 0x1d：buddy 名 / 发送者名）
    char m_type;                    // offset 0x35（封禁类型 1/2/3，0=正常投递）
    unsigned char m_msgLen;         // offset 0x36（原始：movzx 零扩展）
    char m_msg[0x100];              // offset 0x37

    Packet_Monitor_Other_Channel_Chat_ToUser() : PacketHeader(0x3f3, 0x137) {
        // 初始化行为对齐原始二进制（2026-08-06 反汇编验证）
        m_serverId = 0;
        m_idByChannel = -1;
        m_uniqCharNo = 0;
        m_type = 0;
        m_msgLen = 0;
        memset(m_name, 0, sizeof(m_name));
        memset(m_msg, 0, sizeof(m_msg));
    }
} __attribute__((packed));
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Monitor_Other_Channel_Chat_ToUser, 0x137);

#endif  // REDNF_PACKET_MONITOR_OTHER_CHANNEL_CHAT_TOUSER_H
