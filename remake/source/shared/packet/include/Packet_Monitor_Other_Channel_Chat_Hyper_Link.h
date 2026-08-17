//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_OTHER_CHANNEL_CHAT_HYPER_LINK_H
#define PACKET_MONITOR_OTHER_CHANNEL_CHAT_HYPER_LINK_H

#include "PacketHeader.h"
#include <string.h>

#pragma pack(push, 1)
class Packet_Monitor_Other_Channel_Chat_Hyper_Link : public PacketHeader {
public:
    int m_chatType;                 // offset 0xa（send_chat_msg_hyper_link case 1：MSG+0xd 聊天类型字节；
                                    //          Dispatcher_ItemHyperLinkMessage::process 写 1）
    int m_id;                       // offset 0xe（process：CUser::get_acc_id）
    char server_id;                 // offset 0x12
    int m_characNo;                 // offset 0x13（CUserCharacInfo::getCurCharacNo）
    int m_recverCharId;             // offset 0x17（MSG+0x10 whisper 目标；monitor ORIG 日志
                                    //          "m_uRecverCharID"、FindUser_CharNo(m_recverCharId)）
    char m_buddyNameLen;            // offset 0x1b（MSG+0x118 好友名长度，memcpy 0x1c 用）
    char buddy_n_user_id_what[30];  // offset 0x1c
    unsigned char m_itemCount;      // offset 0x3a（MSG+0x13b 超链接道具个数；monitor reply.m_itemCount）
    // 原始：0x138 == 3×0x68，按 ORIG 反汇编（循环地址计算拆成 add $0x30 + add $0xb）
    // 还原为 2D 数组形态（布局与 char[0x138] 完全一致，仅类型形态影响代码生成）。
    char m_items[3][0x68];          // offset 0x3b（MSG+0x13c 起 0x68×n 超链接道具）
    unsigned char m_msgLen;         // offset 0x173（MSG+0x14 消息长度；monitor 日志 "m_msgLen"）
    char m_msg[0x100];              // offset 0x174（MSG+0x18 消息内容）
    Packet_Monitor_Other_Channel_Chat_Hyper_Link() : PacketHeader(0x2719, 0x274)
    {
    m_id = 0;
    server_id = 0;
    m_characNo = 0;
    m_recverCharId = 0;
    m_buddyNameLen = 0;
    m_itemCount = 0;
    m_msgLen = 0;
    memset(buddy_n_user_id_what, 0, sizeof(buddy_n_user_id_what));
    memset(m_items, 0, sizeof(m_items));
    memset(m_msg, 0, sizeof(m_msg));
    };
} __attribute__((packed));
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Monitor_Other_Channel_Chat_Hyper_Link, 0x274);

#endif  // PACKET_MONITOR_OTHER_CHANNEL_CHAT_HYPER_LINK_H
