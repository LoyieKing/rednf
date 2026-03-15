//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_WEB_NOTIFY_MESSAGE_TO_GUILD_H
#define PACKET_WEB_NOTIFY_MESSAGE_TO_GUILD_H

#include "PacketHeader.h"

class Packet_Web_Notify_Message_To_Guild : public PacketHeader {
public:
    Packet_Web_Notify_Message_To_Guild(): PacketHeader(0x42d,0x74) {};
};


#endif //PACKET_WEB_NOTIFY_MESSAGE_TO_GUILD_H
