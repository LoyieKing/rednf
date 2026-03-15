//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MAX_LEVEL_BROADCAST_H
#define PACKET_MONITOR_MAX_LEVEL_BROADCAST_H

#include "PacketHeader.h"

class Packet_Monitor_Max_Level_BroadCast : public PacketHeader {
public:
    Packet_Monitor_Max_Level_BroadCast(): PacketHeader(0x177e,0x28) {};
};


#endif //PACKET_MONITOR_MAX_LEVEL_BROADCAST_H
