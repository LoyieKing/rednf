#ifndef DNF_GAME_PACKET_SYNC_GM_LIST_H_
#define DNF_GAME_PACKET_SYNC_GM_LIST_H_

// Packet_Sync_GM_List：ORIG 弱内联 ctor W @0x082a40ac，sizeof 0x70。
// ctor 反汇编：PacketHeader(this, 0x1f4d, 0x70)；movb $0,(this+0xa)；
// movb $0,(this+0xb)；memset(this+0xc, 0, 0x50)（20 个 int）；
// memset(this+0x5c, 0, 0x14)（20 字节名字区）。
// ORIG SendGMList_To_UpperServer(0x08299d54) 证实字段语义：
// +0xa = m_pageNo（byte，包尾 ++）、+0xb = m_gmCount（byte，满 20 置 0x14）、
// +0xc = m_gmIds[20]（int，写入 -0x80(%ebp,%ebx,4)）、
// +0x5c = m_gmNames[20]（char，写入 -0x30(%ebp,%ebx,1)）。
#include <string.h>

#include "PacketHeader.h"

class Packet_Sync_GM_List : public PacketHeader
{
public:
    Packet_Sync_GM_List()
        : PacketHeader(0x1f4d, 0x70), m_pageNo(0), m_gmCount(0)
    {
        memset(m_gmIds, 0, sizeof(m_gmIds));
        memset(m_gmNames, 0, sizeof(m_gmNames));
    }

    char m_pageNo;       // +0xa
    char m_gmCount;      // +0xb
    int m_gmIds[20];     // +0xc（0x50）
    char m_gmNames[20];  // +0x5c（0x14）
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_SYNC_GM_LIST_H_
