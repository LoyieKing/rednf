//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_WEB_NOTIFY_MESSAGE_TO_GUILD_H
#define PACKET_WEB_NOTIFY_MESSAGE_TO_GUILD_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Web_Notify_Message_To_Guild : public PacketHeader {
public:
    unsigned int m_guildKey;  // +0xa
    unsigned char m_mode;     // +0xe
    char m_payload[0x66];     // +0xf
    Packet_Web_Notify_Message_To_Guild() : PacketHeader(0x42d, 0x74),
        m_guildKey(0), m_mode(0)
    {
    memset((char*)this + 0xf, 0, 0x65);  // m_payload
    };
} __attribute__((packed));


#endif //PACKET_WEB_NOTIFY_MESSAGE_TO_GUILD_H
