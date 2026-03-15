//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CALL_MEMBER_LIST_H
#define PACKET_MONITOR_CALL_MEMBER_LIST_H

#include "PacketHeader.h"

class Packet_Monitor_Call_Member_List : public PacketHeader {
public:
    Packet_Monitor_Call_Member_List() : PacketHeader(0x4be, 0x12){};
};

#endif  // PACKET_MONITOR_CALL_MEMBER_LIST_H
