//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MEMBER_CHAT_H
#define PACKET_MONITOR_MEMBER_CHAT_H

#include "PacketHeader.h"

class Packet_Monitor_Member_Chat : public PacketHeader {
public:
    Packet_Monitor_Member_Chat() : PacketHeader(0x4bf, 0x10f){};
};

#endif  // PACKET_MONITOR_MEMBER_CHAT_H
