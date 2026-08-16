//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_NOTICE_MESSAGE_H
#define PACKET_MONITOR_NOTICE_MESSAGE_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Notice_Message : public PacketHeader {
public:
    unsigned char m_len;            // +0xa
    char m_text[0x100];             // +0xb
    Packet_Monitor_Notice_Message() : PacketHeader(0x9dc, 0x10b)
    {
    memset(m_text, 0, sizeof(m_text));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Notice_Message, 0x10b);

#endif  // PACKET_MONITOR_NOTICE_MESSAGE_H
