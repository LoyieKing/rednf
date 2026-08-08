//
// df_guild_r — 向频道发送公会全部成员列表（首包）
//

#ifndef PACKET_MONITOR_CALL_GUILD_ALL_MEMBERS_TO_CHANNEL_H
#define PACKET_MONITOR_CALL_GUILD_ALL_MEMBERS_TO_CHANNEL_H

#include "PacketHeader.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"

class Packet_Monitor_Call_Guild_All_Members_ToChannel : public PacketHeader {
public:
    Packet_Monitor_Call_Guild_All_Members_ToChannel() : PacketHeader(0x424, 0x17d4)
    {
        *(unsigned int*)((char*)this + 0xa) = 0xffffffff;
        *(unsigned int*)((char*)this + 0xe) = 0;
        *(unsigned int*)((char*)this + 0x12) = 0;
        *(unsigned int*)((char*)this + 0x2d) = 0;
        *(unsigned char*)((char*)this + 0x33) = 0;
        for (int i = 0; i < 96; i++)
        {
            new ((char*)this + 0x34 + i * 0x3f) ST_Guild_Mem_Info;
        }
    }
};

#endif  // PACKET_MONITOR_CALL_GUILD_ALL_MEMBERS_TO_CHANNEL_H
