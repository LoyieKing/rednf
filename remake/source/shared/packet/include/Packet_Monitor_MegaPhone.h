//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MEGAPHONE_H
#define PACKET_MONITOR_MEGAPHONE_H

#include "PacketHeader.h"

class Packet_Monitor_MegaPhone : public PacketHeader {
public:
    Packet_Monitor_MegaPhone() : PacketHeader(0x546, 0x12e){};
};

#endif  // PACKET_MONITOR_MEGAPHONE_H
