//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PACKET_INSERT_PVP_BUDDY_H
#define REDNF_PACKET_INSERT_PVP_BUDDY_H

#include "PacketHeader.h"
#include "STPvPBuddyDBInfo.h"

class Packet_Insert_PvP_Buddy : public PacketHeader {
public:
    STPvPBuddyDBInfo a;
    STPvPBuddyDBInfo b;

    Packet_Insert_PvP_Buddy() : PacketHeader(0x1b63, 0x60){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Insert_PvP_Buddy, 0x60);

#endif  // REDNF_PACKET_INSERT_PVP_BUDDY_H
