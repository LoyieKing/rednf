//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CALL_MEMBER_LIST_H
#define PACKET_MONITOR_CALL_MEMBER_LIST_H

#include "PacketHeader.h"

class Packet_Monitor_Call_Member_List : public PacketHeader {
public:
    unsigned int m_idByChannel;     // +0xa
    unsigned int m_charNo;          // +0xe FindUser_CharNo
    Packet_Monitor_Call_Member_List() : PacketHeader(0x4be, 0x12){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Call_Member_List, 0x12);

#endif  // PACKET_MONITOR_CALL_MEMBER_LIST_H
