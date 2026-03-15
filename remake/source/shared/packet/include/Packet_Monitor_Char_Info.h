//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CHAR_INFO_H
#define PACKET_MONITOR_CHAR_INFO_H

#include "PacketHeader.h"

class Packet_Monitor_Char_Info : public PacketHeader {
public:
    Packet_Monitor_Char_Info() : PacketHeader(0x3ef, 0x3f){};
};

#endif  // PACKET_MONITOR_CHAR_INFO_H
