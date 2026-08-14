//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_NOTIFY_NEW_MAIL_H
#define PACKET_MONITOR_NOTIFY_NEW_MAIL_H

#include "PacketHeader.h"

class Packet_Monitor_Notify_New_Mail : public PacketHeader {
public:
    unsigned int m_charNo;          // +0xa  FindUser_CharNo
    int m_idByChannel;              // +0xe
    Packet_Monitor_Notify_New_Mail() : PacketHeader(0x514, 0x12){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Notify_New_Mail, 0x12);

#endif  // PACKET_MONITOR_NOTIFY_NEW_MAIL_H
