//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_ADD_BUDDY_H
#define PACKET_MONITOR_ADD_BUDDY_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Add_Buddy : public PacketHeader {
public:
    unsigned int m_charNo;          // +0xa  FindUser_CharNo
    unsigned int m_idByChannel;     // +0xe
    char m_name[0x1e];              // +0x12
    Packet_Monitor_Add_Buddy() : PacketHeader(0x672, 0x30)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Add_Buddy, 0x30);

#endif  // PACKET_MONITOR_ADD_BUDDY_H
