//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_NOTICE_GUILD_DISMISS_H
#define PACKET_MONITOR_NOTICE_GUILD_DISMISS_H

#include "PacketHeader.h"

class Packet_Monitor_Notice_Guild_Dismiss : public PacketHeader {
public:
    Packet_Monitor_Notice_Guild_Dismiss(): PacketHeader(0x3fc,0xe) {};
};


#endif //PACKET_MONITOR_NOTICE_GUILD_DISMISS_H
