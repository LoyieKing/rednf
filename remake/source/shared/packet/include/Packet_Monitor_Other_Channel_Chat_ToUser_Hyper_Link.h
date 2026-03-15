//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PACKET_MONITOR_OTHER_CHANNEL_CHAT_TOUSER_HYPER_LINK_H
#define REDNF_PACKET_MONITOR_OTHER_CHANNEL_CHAT_TOUSER_HYPER_LINK_H

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link : public PacketHeader {
public:
    int what_0x0a;           // offset 0xa
    char what_0x0e;          // offset 0xe
    int what_0x0f;           // offset 0xf
    int what_0x13;           // offset 0x13
    char what_0x17[30];      // offset 0x17
    char what_0x35;          // offset 0x35
    char what_0x36;          // offset 0x36
    char what_0x37[0x138];   // offset 0x37
    char what_0x16f;         // offset 0x16f
    char what_0x170[0x100];  // offset 0x170

    Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link() : PacketHeader(0x2719, 0x270) {
    }
};
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link, 0x270);

#endif  // REDNF_PACKET_MONITOR_OTHER_CHANNEL_CHAT_TOUSER_HYPER_LINK_H
