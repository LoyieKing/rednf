//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PACKET_MONITOR_OTHER_CHANNEL_CHAT_TOUSER_HYPER_LINK_H
#define REDNF_PACKET_MONITOR_OTHER_CHANNEL_CHAT_TOUSER_HYPER_LINK_H

#include "PacketHeader.h"
#include <cstring>

#pragma pack(push, 1)
class Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link : public PacketHeader {
public:
    int m_senderCharId;      // offset 0xa（monitor ORIG：reply.m_senderCharId = chat->m_chatType）
    char m_serverId;         // offset 0xe（monitor ORIG ctor：m_serverId = 0）
    int m_idByChannel;       // offset 0xf（monitor ORIG：GetIdByChannel；ctor 0xffffffff）
    int m_uniqCharNo;        // offset 0x13（monitor ORIG：GetUniqCharNo）
    char m_name[0x1e];       // offset 0x17（monitor memcpy 0x1d：发送者名/好友名）
    char m_type;             // offset 0x35（封禁类型 1/2/3，0=正常投递）
    unsigned char m_itemCount;  // offset 0x36（monitor ORIG：reply.m_itemCount = chat->m_itemCount）
    // 原始：0x138 == 3×0x68，按 ORIG 反汇编（循环地址计算拆成 add $0x30 + add $0x7）
    // 还原为 2D 数组形态（布局与 char[0x138] 完全一致，仅类型形态影响代码生成）。
    char m_items[3][0x68];   // offset 0x37（monitor ORIG：memcpy(reply.m_items[i], chat->m_items[i], 0x68)）
    unsigned char m_msgLen;  // offset 0x16f（monitor ORIG：reply.m_msgLen = chat->m_msgLen）
    char m_msg[0x100];       // offset 0x170（monitor ORIG：memcpy(reply.m_msg, chat->m_msg, msgLen)）

    Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link() : PacketHeader(0x2719, 0x270) {
        // 初始化行为对齐原始二进制（2026-08-06 反汇编验证）
        m_serverId = 0;
        m_idByChannel = -1;
        m_uniqCharNo = 0;
        m_type = 0;
        m_itemCount = 0;
        m_msgLen = 0;
        memset(m_name, 0, sizeof(m_name));
        memset(m_msg, 0, sizeof(m_msg));
        memset(m_items, 0, sizeof(m_items));
    }
} __attribute__((packed));
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link, 0x270);

#endif  // REDNF_PACKET_MONITOR_OTHER_CHANNEL_CHAT_TOUSER_HYPER_LINK_H
