#ifndef PACKET_ITEM_LIMIT_EDITION_SELL_START_H_
#define PACKET_ITEM_LIMIT_EDITION_SELL_START_H_

#include "PacketHeader.h"

class Packet_Item_Limit_Edition_Sell_Start : public PacketHeader
{
public:
    signed char m_fullLoad;  // +0xa（回显 LoadDataRpy FullLoad）
    int m_count;             // +0xb（条目数）
    struct __attribute__((packed)) Rec
    {
        unsigned int m_ipgno;  // +0
        char m_rest[0x44];     // +4 .. 0x47，步长 0x48
    } m_items[28];             // +0xf
    Packet_Item_Limit_Edition_Sell_Start() : PacketHeader(0x1004, 0x7ef)
    {
        m_count = 0;
        m_fullLoad = 0;
    }
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Item_Limit_Edition_Sell_Start, 0x7ef);

#endif  // PACKET_ITEM_LIMIT_EDITION_SELL_START_H_
