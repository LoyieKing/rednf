//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MAX_LEVEL_BROADCAST_H
#define PACKET_MONITOR_MAX_LEVEL_BROADCAST_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Max_Level_BroadCast : public PacketHeader {
public:
    char m_name[0x1e];  // +0xa..+0x27  _check_level_up/SetCharacLevel: strncpy(getCurCharacName, 0x1e)

    Packet_Monitor_Max_Level_BroadCast() : PacketHeader(0x177e, 0x28)
    {
    memset(m_name, 0, sizeof(m_name));
    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Monitor_Max_Level_BroadCast, 0x28);


#endif //PACKET_MONITOR_MAX_LEVEL_BROADCAST_H
