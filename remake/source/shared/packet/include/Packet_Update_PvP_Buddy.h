//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PACKET_UPDATE_PVP_BUDDY_H
#define REDNF_PACKET_UPDATE_PVP_BUDDY_H

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_Update_PvP_Buddy : public PacketHeader {
public:
    char server_id;       // offset 10
    int charac_no;        // offset 0xb
    char variable_what1;  // offset 0xf
    char variable_what2;  // offset 0x10

    Packet_Update_PvP_Buddy() : PacketHeader(0x1b65, 0x11) {
    }
};
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Update_PvP_Buddy, 0x11);

#endif  // REDNF_PACKET_UPDATE_PVP_BUDDY_H
