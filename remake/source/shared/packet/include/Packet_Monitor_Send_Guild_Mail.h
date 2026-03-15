//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_SEND_GUILD_MAIL_H
#define PACKET_MONITOR_SEND_GUILD_MAIL_H

#include "PacketHeader.h"

class Packet_Monitor_Send_Guild_Mail : public PacketHeader {
public:
    Packet_Monitor_Send_Guild_Mail() : PacketHeader(0x432, 0x112){};
};

#endif  // PACKET_MONITOR_SEND_GUILD_MAIL_H
