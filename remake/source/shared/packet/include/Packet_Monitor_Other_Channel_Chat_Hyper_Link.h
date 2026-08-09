//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_OTHER_CHANNEL_CHAT_HYPER_LINK_H
#define PACKET_MONITOR_OTHER_CHANNEL_CHAT_HYPER_LINK_H

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_Monitor_Other_Channel_Chat_Hyper_Link : public PacketHeader {
public:
    int what_0x0a;                  // offset 0xa
    int m_id;                       // offset 0xe
    char server_id;                 // offset 0x12
    int what_0x13;                  // offset 0x13
    int what_0x17;                  // offset 0x17
    char what_0x1b;                 // offset 0x1b
    char buddy_n_user_id_what[30];  // offset 0x1c
    unsigned char what_0x3a;        // offset 0x3a（原始：movzx 零扩展）
    char what_0x3b[0x138];          // offset 0x3b
    unsigned char what_0x173;       // offset 0x173（原始：movzx 零扩展）
    char what_0x174[0x100];         // offset 0x174

    Packet_Monitor_Other_Channel_Chat_Hyper_Link()
        : PacketHeader(0x2719, 0x274){};
} __attribute__((packed));
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Monitor_Other_Channel_Chat_Hyper_Link, 0x274);

#endif  // PACKET_MONITOR_OTHER_CHANNEL_CHAT_HYPER_LINK_H
