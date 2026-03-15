//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_FORBID_CHAT_BY_MONITOR_H
#define PACKET_FORBID_CHAT_BY_MONITOR_H

#include "PacketHeader.h"

class Packet_Forbid_Chat_By_Monitor : public PacketHeader {
public:
    Packet_Forbid_Chat_By_Monitor() : PacketHeader(0x9dd, 0x30){};
};

#endif  // PACKET_FORBID_CHAT_BY_MONITOR_H
