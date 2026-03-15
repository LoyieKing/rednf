//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REASON_CRASH_DOWN_INFO_H
#define PACKET_REASON_CRASH_DOWN_INFO_H

#include "PacketHeader.h"

class Packet_Reason_Crash_Down_Info : public PacketHeader {
public:
    Packet_Reason_Crash_Down_Info() : PacketHeader(0xc48, 0x1a){};
};

#endif  // PACKET_REASON_CRASH_DOWN_INFO_H
