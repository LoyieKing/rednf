//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_GUILD_CHAT_HYPER_LINK_H
#define PACKET_MONITOR_GUILD_CHAT_HYPER_LINK_H

#include "PacketHeader.h"

class Packet_Monitor_Guild_Chat_Hyper_Link : public PacketHeader {
public:
    Packet_Monitor_Guild_Chat_Hyper_Link(): PacketHeader(0x2718,0x24c) {};
};


#endif //PACKET_MONITOR_GUILD_CHAT_HYPER_LINK_H
