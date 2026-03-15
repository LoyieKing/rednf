//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_SET_SUB_GUILD_MASTER_H
#define PACKET_MONITOR_SET_SUB_GUILD_MASTER_H

#include "PacketHeader.h"

class Packet_Monitor_Set_Sub_Guild_Master : public PacketHeader {
public:
    Packet_Monitor_Set_Sub_Guild_Master(): PacketHeader(0x430,0x31) {};
};


#endif //PACKET_MONITOR_SET_SUB_GUILD_MASTER_H
