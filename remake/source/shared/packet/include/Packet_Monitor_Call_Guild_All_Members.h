//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CALL_GUILD_ALL_MEMBERS_H
#define PACKET_MONITOR_CALL_GUILD_ALL_MEMBERS_H

#include "PacketHeader.h"

class Packet_Monitor_Call_Guild_All_Members : public PacketHeader {
public:
    Packet_Monitor_Call_Guild_All_Members(): PacketHeader(0x424,0x12) {};
};


#endif //PACKET_MONITOR_CALL_GUILD_ALL_MEMBERS_H
