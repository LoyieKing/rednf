//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MEMBER_ENTER_REPLY_H
#define PACKET_MONITOR_MEMBER_ENTER_REPLY_H

#include "PacketHeader.h"

class Packet_Monitor_Member_Enter_Reply : public PacketHeader {
public:
    unsigned int m_idByChannel;     // +0xa
    unsigned int m_charNo;          // +0xe FindUser_CharNo
    unsigned char m_code;           // +0x12
    Packet_Monitor_Member_Enter_Reply(): PacketHeader(0x4b9,0x13) {};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Member_Enter_Reply, 0x13);

#endif //PACKET_MONITOR_MEMBER_ENTER_REPLY_H
