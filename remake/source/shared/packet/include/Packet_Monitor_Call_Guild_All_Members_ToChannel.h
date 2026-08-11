//
// df_guild_r — 向频道发送公会全部成员列表（首包）
//

#ifndef PACKET_MONITOR_CALL_GUILD_ALL_MEMBERS_TO_CHANNEL_H
#define PACKET_MONITOR_CALL_GUILD_ALL_MEMBERS_TO_CHANNEL_H

#include "PacketHeader.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"

class Packet_Monitor_Call_Guild_All_Members_ToChannel : public PacketHeader {
public:
    Packet_Monitor_Call_Guild_All_Members_ToChannel() : PacketHeader(0x424, 0x17d4),
        m_fieldA(0xffffffff), m_fieldB(0), m_fieldC(0), m_fieldD(0), m_fieldE(0)
    {
        memset((char*)this + 0x16, 0, 0x17);
    }
    unsigned int m_fieldA;            // +0xa
    unsigned int m_fieldB;            // +0xe
    unsigned int m_fieldC;            // +0x12
    char m_field16[0x17];             // +0x16..+0x2c
    unsigned int m_fieldD;            // +0x2d
    char m_pad[2];                    // +0x31..+0x32
    unsigned char m_fieldE;           // +0x33
    ST_Guild_Mem_Info m_members[96];  // +0x34
} __attribute__((packed));

#endif  // PACKET_MONITOR_CALL_GUILD_ALL_MEMBERS_TO_CHANNEL_H
