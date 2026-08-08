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
    char m_data[0x3f];
};

inline ST_Guild_Mem_Info::ST_Guild_Mem_Info()
{
    m_data[0] = (char)0xff;
    m_data[1] = (char)0xff;
    *(unsigned short*)(m_data + 2) = 0xffff;
    memset(m_data + 4, 0, 0x1e);
    memset(m_data + 0x22, 0, 0x15);
    m_data[0x37] = (char)0xff;
    m_data[0x38] = 1;
    m_data[0x39] = 0;
    m_data[0x3a] = 0;
    *(unsigned int*)(m_data + 0x3b) = 0;
}

class Packet_Monitor_Call_Guild_Members_ToChannel : public PacketHeader {
public:
    Packet_Monitor_Call_Guild_Members_ToChannel() : PacketHeader(0x401, 0x17d4)
    {
        *(unsigned int*)((char*)this + 0xa) = 0xffffffff;
        *(unsigned int*)((char*)this + 0xe) = 0;
        *(unsigned int*)((char*)this + 0x12) = 0;
        *(unsigned char*)((char*)this + 0x2d) = 0;
        *(unsigned int*)((char*)this + 0x2e) = 0;
        for (int i = 0; i < 96; i++)
        {
            new ((char*)this + 0x34 + i * 0x3f) ST_Guild_Mem_Info;
        }
        memset((char*)this + 0x16, 0, 0x17);
    }
};

#endif  // PACKET_MONITOR_CALL_GUILD_MEMBERS_TO_CHANNEL_H
