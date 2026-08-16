//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CHANGE_CHAR_NAME_H
#define PACKET_CHANGE_CHAR_NAME_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Change_Char_Name : public PacketHeader {
public:
    unsigned int m_idByChannel;     // +0xa
    unsigned int m_charNo;          // +0xe
    unsigned int m_field12;         // +0x12
    char m_name[0x1e];              // +0x16
    Packet_Change_Char_Name() : PacketHeader(0x451, 0x34),
        m_idByChannel(0), m_charNo(0), m_field12(0)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Change_Char_Name, 0x34);

#endif  // PACKET_CHANGE_CHAR_NAME_H
