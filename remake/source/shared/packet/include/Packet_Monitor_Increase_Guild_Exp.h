//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_INCREASE_GUILD_EXP_H
#define PACKET_MONITOR_INCREASE_GUILD_EXP_H

#include "PacketHeader.h"

class Packet_Monitor_Increase_Guild_Exp : public PacketHeader {
public:
    Packet_Monitor_Increase_Guild_Exp(): PacketHeader(0x40e,0x1c) {};
};


#endif //PACKET_MONITOR_INCREASE_GUILD_EXP_H
