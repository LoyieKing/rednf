//
// df_guild_r — 向频道发送公会成员列表（首包）
//

#ifndef PACKET_MONITOR_CALL_GUILD_MEMBERS_TO_CHANNEL_H
#define PACKET_MONITOR_CALL_GUILD_MEMBERS_TO_CHANNEL_H

#include <string.h>

#include "PacketHeader.h"

// ST_Guild_Mem_Info：0x3f，成员记录（job/growth/level/name/...）
struct ST_Guild_Mem_Info
{
    ST_Guild_Mem_Info();
    union {
        char m_data[0x3f];
        struct __attribute__((packed)) {
            char m_job;                    // +0（df_guild_r ReplyGuildMembers: rec[0] = m->GetJob()）
            char m_growthType;             // +1（rec[1] = m->GetGrowthType()）
            unsigned short m_level;        // +2（*(ushort*)(rec+2) = m->GetLevel()）
            char m_name[0x1d];             // +4..+0x20（memcpy(rec+4, m->GetCharName(), 0x1d)）
            char m_pad21;                  // +0x21
            char m_guildMemDBInfo[0x14];   // +0x22..+0x35（memcpy(rec+0x22, m->GetGuildMemDBInfo(), 0x14)）
            char m_pad36;                  // +0x36
            char m_channel;                // +0x37（rec[0x37] = m->GetGameServer()->GetChannelNo()）
            char m_sex;                    // +0x38（rec[0x38] = m->GetSex()）
            char m_blackUser;              // +0x39（rec[0x39] = m->IsBlackUser()?1:0）
            char m_grade;                  // +0x3a（rec[0x3a] = m->GetGuildMemDBInfo()->m_grade）
            unsigned int m_pad3b;          // +0x3b..+0x3e（ctor 清零，发送端不写）
        } f;
    };
};

inline ST_Guild_Mem_Info::ST_Guild_Mem_Info()
{
    m_data[0] = (char)0xff;
    m_data[1] = (char)0xff;
    f.m_level = 0xffff;
    f.m_channel = (char)0xff;
    f.m_sex = 1;
    f.m_blackUser = 0;
    f.m_grade = 0;
    f.m_pad3b = 0;
    memset(m_data + 4, 0, 0x1e);
    memset(m_data + 0x22, 0, 0x15);
}

class Packet_Monitor_Call_Guild_Members_ToChannel : public PacketHeader {
public:
    Packet_Monitor_Call_Guild_Members_ToChannel() : PacketHeader(0x401, 0x17d4),
        m_channel(0xffffffff), m_charNo(0), m_guildKey(0), m_count(0), m_guildPoint(0)
    {
        memset((char*)this + 0x16, 0, 0x17);
    }
    unsigned int m_channel;           // +0xa（家族证据 All_Next: user->GetIdByChannel；ctor 默认 0xffffffff）
    unsigned int m_charNo;            // +0xe（家族证据 All_Next: user->GetUniqCharNo）
    unsigned int m_guildKey;          // +0x12（家族证据 All_Next: m_guildKey）
    char m_guildName[0x17];           // +0x16..+0x2c（df_guild_r CGuild::ReplyGuildMembers: memcpy(buf+0x16, m_dbInfo.m_info.m_guildName, 0x16)；All_Members 同型）
    unsigned char m_count;            // +0x2d（ReplyGuildMembers: 本批成员数 count，SendToGameserver 前写）
    unsigned int m_guildPoint;        // +0x2e（ReplyGuildMembers: m_dbInfo.m_info.m_guildPoint）
    unsigned short m_totalCnt;        // +0x32..+0x33（ReplyGuildMembers: m_dbInfo.m_info.m_totalCnt）
    ST_Guild_Mem_Info m_members[96];  // +0x34
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Call_Guild_Members_ToChannel, 0x17d4);

#endif  // PACKET_MONITOR_CALL_GUILD_MEMBERS_TO_CHANNEL_H
