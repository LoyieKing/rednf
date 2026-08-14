//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_DEL_BUDDY_H
#define PACKET_MONITOR_DEL_BUDDY_H

#include "PacketHeader.h"

class Packet_Monitor_Del_Buddy : public PacketHeader {
public:
    unsigned int m_charNo;          // +0xa  FindUser_CharNo
    unsigned int m_idByChannel;     // +0xe
    char m_name[0x1e];              // +0x12
    Packet_Monitor_Del_Buddy() : PacketHeader(0x674, 0x30){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Del_Buddy, 0x30);

#endif  // PACKET_MONITOR_DEL_BUDDY_H
