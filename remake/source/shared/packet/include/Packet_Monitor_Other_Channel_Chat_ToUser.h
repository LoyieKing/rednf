//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PACKET_MONITOR_OTHER_CHANNEL_CHAT_TOUSER_H
#define REDNF_PACKET_MONITOR_OTHER_CHANNEL_CHAT_TOUSER_H

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_Monitor_Other_Channel_Chat_ToUser : public PacketHeader {
public:
    int what_0x0a;                  // offset 0x0a
    char server_id;                 // offset 0x0e
    int sTGameUserInfo_what3_0x05;  // offset 0x0f
    int charac_no;                  // offset 0x13
    char buddy_n_user_id_what[30];  // offset 0x17
    char errorCode;                 // offset 0x35
    char chatLength;                // offset 0x36
    char chatContent[0x100];        // offset 0x37

    Packet_Monitor_Other_Channel_Chat_ToUser() : PacketHeader(0x3f3, 0x137) {
    }
};
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Monitor_Other_Channel_Chat_ToUser, 0x137);

#endif  // REDNF_PACKET_MONITOR_OTHER_CHANNEL_CHAT_TOUSER_H
