//
// df_guild_r — 向频道发送公会成员列表（后续批）
//

#ifndef PACKET_MONITOR_CALL_GUILD_MEMBERS_TO_CHANNEL_NEXT_H
#define PACKET_MONITOR_CALL_GUILD_MEMBERS_TO_CHANNEL_NEXT_H

#include "PacketHeader.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"

class Packet_Monitor_Call_Guild_Members_ToChannel_Next : public PacketHeader {
public:
    Packet_Monitor_Call_Guild_Members_ToChannel_Next() : PacketHeader(0x423, 0x17b7),
        m_channel(0xffffffff), m_charNo(0), m_guildKey(0), m_count(0)
    {
    }
    unsigned int m_channel;           // +0xa（df_guild_r CGuild::ReplyGuildMembers: user->GetIdByChannel）
    unsigned int m_charNo;            // +0xe（ReplyGuildMembers: user->GetUniqCharNo）
    unsigned int m_guildKey;          // +0x12（ReplyGuildMembers: m_guildKey）
    unsigned char m_count;            // +0x16（ReplyGuildMembers: 本批成员数 cnt2）
    ST_Guild_Mem_Info m_members[96];  // +0x17
} __attribute__((packed));

#endif  // PACKET_MONITOR_CALL_GUILD_MEMBERS_TO_CHANNEL_NEXT_H
