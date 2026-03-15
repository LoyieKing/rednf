//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CALL_GUILD_LEVEL_UP_H
#define PACKET_MONITOR_CALL_GUILD_LEVEL_UP_H

#include "PacketHeader.h"

class Packet_Monitor_Call_Guild_Level_Up : public PacketHeader {
public:
    Packet_Monitor_Call_Guild_Level_Up(): PacketHeader(0x408,0x1b) {};
};


#endif //PACKET_MONITOR_CALL_GUILD_LEVEL_UP_H
