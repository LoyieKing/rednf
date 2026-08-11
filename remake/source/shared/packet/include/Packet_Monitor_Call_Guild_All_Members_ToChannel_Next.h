//
// df_guild_r — 向频道发送公会全部成员列表（后续批）
//

#ifndef PACKET_MONITOR_CALL_GUILD_ALL_MEMBERS_TO_CHANNEL_NEXT_H
#define PACKET_MONITOR_CALL_GUILD_ALL_MEMBERS_TO_CHANNEL_NEXT_H

#include "PacketHeader.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"

class Packet_Monitor_Call_Guild_All_Members_ToChannel_Next : public PacketHeader {
public:
    Packet_Monitor_Call_Guild_All_Members_ToChannel_Next() : PacketHeader(0x428, 0x17b7),
        m_fieldA(0xffffffff), m_fieldB(0), m_fieldC(0), m_fieldD(0)
    {
    }
    unsigned int m_fieldA;            // +0xa
    unsigned int m_fieldB;            // +0xe
    unsigned int m_fieldC;            // +0x12
    unsigned char m_fieldD;           // +0x16
    ST_Guild_Mem_Info m_members[96];  // +0x17
} __attribute__((packed));

#endif  // PACKET_MONITOR_CALL_GUILD_ALL_MEMBERS_TO_CHANNEL_NEXT_H
