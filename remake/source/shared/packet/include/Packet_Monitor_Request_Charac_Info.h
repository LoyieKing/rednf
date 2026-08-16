//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_REQUEST_CHARAC_INFO_H
#define PACKET_MONITOR_REQUEST_CHARAC_INFO_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Request_Charac_Info : public PacketHeader {
public:
    // 包长 0x23：header 10 + name 0x15 + requester 4。handler 从 +0xa 起
    // strncpy 0x1d 个字节（会读过包尾），但 C++ 对象不能按 0x1d 开 name，
    // 否则和 +0x1f 的 requester 重叠，sizeof 对不上 0x23。
    char m_name[0x15];           // +0xa
    unsigned int m_requesterNo;  // +0x1f
    Packet_Monitor_Request_Charac_Info() : PacketHeader(0x27de, 0x23),
        m_requesterNo(0)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Request_Charac_Info, 0x23);


#endif //PACKET_MONITOR_REQUEST_CHARAC_INFO_H
