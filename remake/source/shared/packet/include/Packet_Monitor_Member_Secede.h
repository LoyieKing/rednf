//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MEMBER_SECEDE_H
#define PACKET_MONITOR_MEMBER_SECEDE_H

#include <string.h>

#include "PacketHeader.h"

class Packet_Monitor_Member_Secede : public PacketHeader {
public:
    unsigned int m_idByChannel;     // +0xa
    unsigned int m_uniqCharNo;      // +0xe FindUser_CharNo
    unsigned char m_type;           // +0x12
    char m_name[0x1e];              // +0x13
    Packet_Monitor_Member_Secede() : PacketHeader(0x4bb, 0x31)
    {
        memset(m_name, 0, 0x1e);
    }
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Member_Secede, 0x31);

#endif  // PACKET_MONITOR_MEMBER_SECEDE_H
