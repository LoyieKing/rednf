//
// df_guild_r — 向频道发送公会成员列表（后续批）
//

#ifndef PACKET_MONITOR_CALL_GUILD_MEMBERS_TO_CHANNEL_NEXT_H
#define PACKET_MONITOR_CALL_GUILD_MEMBERS_TO_CHANNEL_NEXT_H

#include "PacketHeader.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"

class Packet_Monitor_Call_Guild_Members_ToChannel_Next : public PacketHeader {
public:
    Packet_Monitor_Call_Guild_Members_ToChannel_Next() : PacketHeader(0x423, 0x17b7)
    {
        *(unsigned int*)((char*)this + 0xa) = 0xffffffff;
        *(unsigned int*)((char*)this + 0xe) = 0;
        *(unsigned int*)((char*)this + 0x12) = 0;
        *(unsigned char*)((char*)this + 0x16) = 0;
        for (int i = 0; i < 96; i++)
        {
            new ((char*)this + 0x17 + i * 0x3f) ST_Guild_Mem_Info;
        }
    }
};

#endif  // PACKET_MONITOR_CALL_GUILD_MEMBERS_TO_CHANNEL_NEXT_H
