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
            char m_b0;            // +0
            char m_b1;            // +1
            unsigned short m_w2;  // +2
            char m_pad4[0x33];    // +4..+0x36（memset 区域）
            char m_b37;           // +0x37
            char m_b38;           // +0x38
            char m_b39;           // +0x39
            char m_b3a;           // +0x3a
            unsigned int m_i3b;   // +0x3b
        } f;
    };
};

inline ST_Guild_Mem_Info::ST_Guild_Mem_Info()
{
    m_data[0] = (char)0xff;
    m_data[1] = (char)0xff;
    f.m_w2 = 0xffff;
    f.m_b37 = (char)0xff;
    f.m_b38 = 1;
    f.m_b39 = 0;
    f.m_b3a = 0;
    f.m_i3b = 0;
    memset(m_data + 4, 0, 0x1e);
    memset(m_data + 0x22, 0, 0x15);
}

class Packet_Monitor_Call_Guild_Members_ToChannel : public PacketHeader {
public:
    Packet_Monitor_Call_Guild_Members_ToChannel() : PacketHeader(0x401, 0x17d4),
        m_fieldA(0xffffffff), m_fieldB(0), m_fieldC(0), m_fieldD(0), m_fieldE(0)
    {
        memset((char*)this + 0x16, 0, 0x17);
    }
    unsigned int m_fieldA;            // +0xa
    unsigned int m_fieldB;            // +0xe
    unsigned int m_fieldC;            // +0x12
    char m_field16[0x17];             // +0x16..+0x2c（body memset 区域）
    unsigned char m_fieldD;           // +0x2d
    unsigned int m_fieldE;            // +0x2e
    char m_pad[2];                    // +0x32..+0x33
    ST_Guild_Mem_Info m_members[96];  // +0x34
} __attribute__((packed));

#endif  // PACKET_MONITOR_CALL_GUILD_MEMBERS_TO_CHANNEL_H
