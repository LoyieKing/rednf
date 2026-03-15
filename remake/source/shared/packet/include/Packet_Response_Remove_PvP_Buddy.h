//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PACKET_RESPONSE_REMOVE_PVP_BUDDY_H
#define REDNF_PACKET_RESPONSE_REMOVE_PVP_BUDDY_H

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_Response_Remove_PvP_Buddy : public PacketHeader {
public:
    int charac_no;                          // offset 10
    int sTGameUserInfo_what3_0x05;          // offset 14
    char server_id;               // offset 0x12
    char buddy_n_user_id_what[0x1e];        // offset 0x13
    unsigned char error_code_what_0x31;  // offset 0x31

    Packet_Response_Remove_PvP_Buddy() : PacketHeader(0x1b61, 0x32) {
    }
};
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Response_Remove_PvP_Buddy, 0x32);

#endif  // REDNF_PACKET_RESPONSE_REMOVE_PVP_BUDDY_H
