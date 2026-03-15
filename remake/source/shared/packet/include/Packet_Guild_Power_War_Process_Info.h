//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_POWER_WAR_PROCESS_INFO_H
#define PACKET_GUILD_POWER_WAR_PROCESS_INFO_H

#include "PacketHeader.h"

class Packet_Guild_Power_War_Process_Info : public PacketHeader {
public:
    Packet_Guild_Power_War_Process_Info(): PacketHeader(0x6e0,0xe) {};
};


#endif //PACKET_GUILD_POWER_WAR_PROCESS_INFO_H
