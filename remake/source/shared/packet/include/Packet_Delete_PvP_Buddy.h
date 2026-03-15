//
// Created by loyieking on 24-3-24.
//

#ifndef REDNF_PACKET_DELETE_PVP_BUDDY_H
#define REDNF_PACKET_DELETE_PVP_BUDDY_H

#include "PacketHeader.h"

class Packet_Delete_PvP_Buddy : public PacketHeader {
public:
    char user_server_id;
    uint user_charac_no;
    char buddy_server_id;
    uint buddy_charac_no;

    Packet_Delete_PvP_Buddy() : PacketHeader(0x1b64, 0x14){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Delete_PvP_Buddy, 0x14);

#endif  // REDNF_PACKET_DELETE_PVP_BUDDY_H
