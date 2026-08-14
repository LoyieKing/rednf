//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_UDP_REPLY_USERINFO_H
#define PACKET_MONITOR_UDP_REPLY_USERINFO_H

#include "PacketHeader.h"

struct ST_MonitorReplyUserInfo
{
    unsigned int m_dbid;        // +0x0  FindUser / CreateUser dbid
    unsigned int m_charNo;      // +0x4
    int m_channel;              // +0x8  CreateUser channel
    char m_padC[4];             // +0xc
    char m_job;                 // +0x10
    char m_grade;               // +0x11
    short m_level;              // +0x12
    char m_name[0x1e];          // +0x14
    int m_memberKey;            // +0x32
    char m_pad36[0x10];         // +0x36
    unsigned char m_sex;        // +0x46
    char m_ssn[7];              // +0x47
} __attribute__((packed));

class Packet_Monitor_UDP_Reply_UserInfo : public PacketHeader {
public:
    unsigned char m_count;                      // +0xa
    unsigned char m_channel;                    // +0xb
    ST_MonitorReplyUserInfo m_items[75];        // +0xc
    Packet_Monitor_UDP_Reply_UserInfo(): PacketHeader(0x3ea,0x16e6) {};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_UDP_Reply_UserInfo, 0x16e6);

#endif //PACKET_MONITOR_UDP_REPLY_USERINFO_H
