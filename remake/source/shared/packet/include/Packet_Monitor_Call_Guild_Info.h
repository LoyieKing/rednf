//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CALL_GUILD_INFO_H
#define PACKET_MONITOR_CALL_GUILD_INFO_H

#include "PacketHeader.h"

class Packet_Monitor_Call_Guild_Info : public PacketHeader {
public:
    Packet_Monitor_Call_Guild_Info() : PacketHeader(0x40d, 0x18f){};
};

#endif  // PACKET_MONITOR_CALL_GUILD_INFO_H
