//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PACKET_NOTICE_PVP_BUDDY_IN_OUT_H
#define REDNF_PACKET_NOTICE_PVP_BUDDY_IN_OUT_H

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_Notice_PvP_Buddy_In_Out : public PacketHeader {
public:
    int what_0xa;        // offset 0xa
    int what_0xe;        // offset 0xe
    char what_0x12;      // offset 0x12
    char channel_no;      // offset 0x13
    char server_id;      // offset 0x14
    char buddy_n_user_id_what[30];  // offset 0x15

    Packet_Notice_PvP_Buddy_In_Out() : PacketHeader(0x1b62, 0x33) {
    }
};
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Notice_PvP_Buddy_In_Out, 0x33);

#endif  // REDNF_PACKET_NOTICE_PVP_BUDDY_IN_OUT_H
