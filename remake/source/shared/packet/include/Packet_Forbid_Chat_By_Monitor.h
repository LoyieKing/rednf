//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_FORBID_CHAT_BY_MONITOR_H
#define PACKET_FORBID_CHAT_BY_MONITOR_H

#include "PacketHeader.h"

class Packet_Forbid_Chat_By_Monitor : public PacketHeader {
public:
    unsigned int m_secs;            // +0xa
    char m_padE[4];                 // +0xe
    char m_name[0x1e];              // +0x12
    Packet_Forbid_Chat_By_Monitor() : PacketHeader(0x9dd, 0x30){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Forbid_Chat_By_Monitor, 0x30);

#endif  // PACKET_FORBID_CHAT_BY_MONITOR_H
