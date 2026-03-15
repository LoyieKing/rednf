//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_DEL_BUDDY_H
#define PACKET_MONITOR_DEL_BUDDY_H

#include "PacketHeader.h"

class Packet_Monitor_Del_Buddy : public PacketHeader {
public:
    Packet_Monitor_Del_Buddy() : PacketHeader(0x674, 0x30){};
};

#endif  // PACKET_MONITOR_DEL_BUDDY_H
