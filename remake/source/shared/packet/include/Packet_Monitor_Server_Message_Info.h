//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_SERVER_MESSAGE_INFO_H
#define PACKET_MONITOR_SERVER_MESSAGE_INFO_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Server_Message_Info : public PacketHeader {
public:
    char m_channel;  // +a（dispatch_sig：CEnvironment::get_channel_no）
    char m_kind;  // +b（dispatch_sig：源结构+8；日志 kind:%d）
    char m_index;  // +c（dispatch_sig：源结构+9；日志 index:%d）
    int m_msgLen;  // +d（dispatch_sig：源结构+0xc；日志 msg_len:%d）
    char m_msg[0x32];  // +11..+42（dispatch_sig：strncpy 0x32 自源结构+0x10；日志 msg:%s）
    int m_nameLen;  // +43（dispatch_sig：源结构+0x44；日志 name_len:%d）
    char m_name[0x14];  // +47..+5a（dispatch_sig：strncpy 0x14 自源结构+0x48；日志 name:%s）
    int m_time;  // +5b（dispatch_sig：源结构+0x5c；日志 time:%d）

    Packet_Monitor_Server_Message_Info() : PacketHeader(0xfae, 0x5f),
        m_channel(0), m_kind(0), m_index(0), m_msgLen(0), m_nameLen(0), m_time(0)
    {
    memset(m_msg, 0, sizeof(m_msg));
    memset(m_name, 0, sizeof(m_name));
    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Monitor_Server_Message_Info, 0x5f);


#endif //PACKET_MONITOR_SERVER_MESSAGE_INFO_H
