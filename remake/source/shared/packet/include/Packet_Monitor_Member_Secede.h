//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MEMBER_SECEDE_H
#define PACKET_MONITOR_MEMBER_SECEDE_H

#include <string.h>

#include "PacketHeader.h"

class Packet_Monitor_Member_Secede : public PacketHeader {
public:
    Packet_Monitor_Member_Secede() : PacketHeader(0x4bb, 0x31)
    {
        memset((char*)this + 0x13, 0, 0x1e);
    }
} __attribute__((packed));

#endif  // PACKET_MONITOR_MEMBER_SECEDE_H
