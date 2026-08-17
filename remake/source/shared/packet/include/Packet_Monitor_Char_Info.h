//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CHAR_INFO_H
#define PACKET_MONITOR_CHAR_INFO_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Char_Info : public PacketHeader {
public:
    union {
        unsigned int m_dbid;            // +0xa  FindUser
        unsigned int m_serverNo;        // +0xa  Guild OnCharLogin
    };
    unsigned char m_channel;        // +0xe
    union {
        unsigned int m_charNo;          // +0xf
        unsigned int m_charKey;         // +0xf  Guild OnCharLogin
    };
    unsigned int m_guildKey;        // +0x13
    char m_job;                     // +0x17
    union {
        char m_grade;                   // +0x18
        char m_pvp;                     // +0x18  Guild OnCharLogin
    };
    short m_level;                  // +0x19
    unsigned int m_memberKey;       // +0x1b
    char m_name[0x1e];              // +0x1f
    union {
        char m_channelType;             // +0x3d  insert_game_world: GameWorld::GetChannelType
        unsigned char m_war;            // +0x3d  Guild OnCharLogin
    };
    char m_returnUser;              // +0x3e
    Packet_Monitor_Char_Info() : PacketHeader(0x3ef, 0x3f),
        m_dbid(0), m_channel(255), m_charNo(0), m_guildKey(0), m_job(255), m_grade(255), m_level(65535), m_memberKey(0), m_returnUser(0)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Char_Info, 0x3f);

#endif  // PACKET_MONITOR_CHAR_INFO_H
