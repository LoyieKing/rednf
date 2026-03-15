//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PACKET_NOTICE_ADD_PVP_BUDDY_RESULT_H
#define REDNF_PACKET_NOTICE_ADD_PVP_BUDDY_RESULT_H

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_Notice_Add_PvP_Buddy_Result : public PacketHeader {
public:
    int charac_no;                          // offset 10
    int sTGameUserInfo_what3_0x05;          // offset 14
    char reason_what_18_0x12;               // offset 0x12
    char server_id;                         // offset 0x13
    char buddy_n_user_id_what[0x1e];        // offset 0x14
    unsigned char error_code_what_50_0x32;  // offset 0x32

    Packet_Notice_Add_PvP_Buddy_Result() : PacketHeader(0x1b5f, 0x33) {
    }
};

#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Notice_Add_PvP_Buddy_Result, 0x33);

#endif  // REDNF_PACKET_NOTICE_ADD_PVP_BUDDY_RESULT_H
