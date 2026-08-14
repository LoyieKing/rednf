//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CHAR_CHANGABLE_INFO_H
#define PACKET_MONITOR_CHAR_CHANGABLE_INFO_H

#include "PacketHeader.h"

class Packet_Monitor_Char_Changable_Info : public PacketHeader {
public:
    unsigned int m_dbid;            // +0xa  FindUser
    char m_padE;                    // +0xe
    union {
        short m_level;              // +0xf
        short m_field_f;            // +0xf  Guild OnUpdateChangableCharInfo
    };
    union {
        char m_flag;                // +0x11
        char m_field_11;            // +0x11
    };
    Packet_Monitor_Char_Changable_Info(): PacketHeader(0x3f5,0x12) {};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Char_Changable_Info, 0x12);

#endif //PACKET_MONITOR_CHAR_CHANGABLE_INFO_H
