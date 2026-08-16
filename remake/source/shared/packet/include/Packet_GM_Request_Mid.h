//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GM_REQUEST_MID_H
#define PACKET_GM_REQUEST_MID_H

#include <string.h>

#include "PacketHeader.h"

class Packet_GM_Request_Mid : public PacketHeader {
public:
    unsigned int m_gmId;            // +0xa
    unsigned int m_dbid;            // +0xe  请求方 FindUser；回包写目标 dbid
    unsigned int m_charNo;          // +0x12 回包写目标 uniqCharNo
    char m_name[0x1e];              // +0x16
    Packet_GM_Request_Mid() : PacketHeader(0x76c, 0x34)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_GM_Request_Mid, 0x34);

#endif  // PACKET_GM_REQUEST_MID_H
