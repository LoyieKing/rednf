//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_OTHER_CHANNEL_CHAT_H
#define PACKET_MONITOR_OTHER_CHANNEL_CHAT_H

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_Monitor_Other_Channel_Chat : public PacketHeader {
public:
    int what_0x0a;                  // offset 0x0a
    int m_id;                       // offset 0x0e
    char server_id;                 // offset 0x12
    int what_0x13;                  // offset 0x13
    int what_0x17;                  // offset 0x17
    char what_0x1b;                 // offset 0x1b
    char buddy_n_user_id_what[30];  // offset 0x1c
    char chatLength;                // offset 0x3a
    char chatContent[256];          // offset 0x3b

    Packet_Monitor_Other_Channel_Chat()
        : PacketHeader(0x3f3, 0x13b){};
};
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Monitor_Other_Channel_Chat, 0x13b);

#endif  // PACKET_MONITOR_OTHER_CHANNEL_CHAT_H
