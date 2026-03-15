//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_GUILD_CHAT_H
#define PACKET_MONITOR_GUILD_CHAT_H

#include "PacketHeader.h"

class Packet_Monitor_Guild_Chat : public PacketHeader {
public:
    Packet_Monitor_Guild_Chat() : PacketHeader(0x400, 0x113){};
};

#endif  // PACKET_MONITOR_GUILD_CHAT_H
