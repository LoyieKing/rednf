//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_OTHER_CHANNEL_CHAT_H
#define PACKET_MONITOR_OTHER_CHANNEL_CHAT_H

#include "PacketHeader.h"
#include <string.h>

#pragma pack(push, 1)
class Packet_Monitor_Other_Channel_Chat : public PacketHeader {
public:
    int m_chatType;                 // offset 0x0a（send_chat_msg param_2 聊天类型；
                                    //          dispatch_sig 按消息类型写 7/0x21/1）
    int m_id;                       // offset 0x0e（CUser::get_acc_id；Community find_user(m_id)）
    char server_id;                 // offset 0x12（好友所在服务器 id）
    int m_characNo;                 // offset 0x13（CUserCharacInfo::getCurCharacNo）
    int m_recverCharId;             // offset 0x17（whisper 接收方 charac no：Monitor OnNoticeOtherChannelChatMsg/
                                    //          OnNoticeBuddyChatMsg FindUser_CharNo(chat->m_recverCharId)，
                                    //          异常串 "packet->m_uRecverCharID"；同族 Hyper_Link 包同证据命名）
    char m_buddyNameLen;            // offset 0x1b（好友名长度，dispatch_sig local_193 / send_chat_msg param_8）
    char buddy_n_user_id_what[30];  // offset 0x1c（好友 user id）
    // 证据：ORIG 异常串将长度字段命名为 m_msgLen（"packet->m_msgLen"），家族 Hyper_Link
    // 亦用 m_msgLen/m_msg；Community/User.cpp 将本字段拷入 ToUser.m_msgLen/m_msg。
    unsigned char m_msgLen;         // offset 0x3a（原始：movzx 零扩展；聊天内容长度）
    char m_msg[256];                // offset 0x3b
    Packet_Monitor_Other_Channel_Chat() : PacketHeader(0x3f3, 0x13b),
        m_id(0), server_id(0), m_characNo(0), m_recverCharId(0), m_buddyNameLen(0), m_msgLen(0)
    {
    memset(buddy_n_user_id_what, 0, sizeof(buddy_n_user_id_what));
    memset(m_msg, 0, sizeof(m_msg));
    };
} __attribute__((packed));
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Monitor_Other_Channel_Chat, 0x13b);

#endif  // PACKET_MONITOR_OTHER_CHANNEL_CHAT_H
