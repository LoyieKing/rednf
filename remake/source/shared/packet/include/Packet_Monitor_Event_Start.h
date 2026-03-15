//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_EVENT_START_H
#define PACKET_MONITOR_EVENT_START_H

#include "PacketHeader.h"

class Packet_Monitor_Event_Start : public PacketHeader {
public:
    Packet_Monitor_Event_Start() : PacketHeader(0x44f, 0x12){};
};

#endif  // PACKET_MONITOR_EVENT_START_H
