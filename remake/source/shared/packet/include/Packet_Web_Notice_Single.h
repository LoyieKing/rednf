//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_WEB_NOTICE_SINGLE_H
#define PACKET_WEB_NOTICE_SINGLE_H

#include "PacketHeader.h"

class Packet_Web_Notice_Single : public PacketHeader {
public:
    unsigned char m_len;            // +0xa
    char m_text[0xff];              // +0xb
    Packet_Web_Notice_Single() : PacketHeader(0x9e0, 0x10a){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Web_Notice_Single, 0x10a);

#endif  // PACKET_WEB_NOTICE_SINGLE_H
