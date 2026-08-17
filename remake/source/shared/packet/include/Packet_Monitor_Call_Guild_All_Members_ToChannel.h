//
// df_guild_r — 向频道发送公会全部成员列表（首包）
//

#ifndef PACKET_MONITOR_CALL_GUILD_ALL_MEMBERS_TO_CHANNEL_H
#define PACKET_MONITOR_CALL_GUILD_ALL_MEMBERS_TO_CHANNEL_H

#include "PacketHeader.h"
#include <string.h>
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"

class Packet_Monitor_Call_Guild_All_Members_ToChannel : public PacketHeader {
public:
    Packet_Monitor_Call_Guild_All_Members_ToChannel() : PacketHeader(0x424, 0x17d4),
        m_channel(0xffffffff), m_charNo(0), m_guildKey(0), m_guildPoint(0), m_count(0)
    {
        memset((char*)this + 0x16, 0, 0x17);
    }
    unsigned int m_channel;           // +0xa  df_guild_r ReplyGuildAllMembers: user->GetIdByChannel
    unsigned int m_charNo;            // +0xe  user->GetUniqCharNo
    unsigned int m_guildKey;          // +0x12  m_guildKey
    char m_guildName[0x17];           // +0x16..+0x2c  memcpy(m_dbInfo.m_info.m_guildName, 0x16)
    unsigned int m_guildPoint;        // +0x2d  m_dbInfo.m_info.m_guildPoint
    unsigned short m_totalCnt;        // +0x31  公会成员总数
    unsigned char m_count;            // +0x33  本批成员数（count）
    ST_Guild_Mem_Info m_members[96];  // +0x34
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Call_Guild_All_Members_ToChannel, 0x17d4);

#endif  // PACKET_MONITOR_CALL_GUILD_ALL_MEMBERS_TO_CHANNEL_H
