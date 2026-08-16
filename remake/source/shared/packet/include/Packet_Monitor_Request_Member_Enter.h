//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_REQUEST_MEMBER_ENTER_H
#define PACKET_MONITOR_REQUEST_MEMBER_ENTER_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Request_Member_Enter : public PacketHeader {
public:
    unsigned int m_idByChannel;     // +0xa
    unsigned int m_charNo;          // +0xe FindUser_CharNo
    char m_name[0x1e];              // +0x12
    Packet_Monitor_Request_Member_Enter() : PacketHeader(0x4b7, 0x30)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Request_Member_Enter, 0x30);

#endif //PACKET_MONITOR_REQUEST_MEMBER_ENTER_H
