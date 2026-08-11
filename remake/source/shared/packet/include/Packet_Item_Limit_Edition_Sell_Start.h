#ifndef PACKET_ITEM_LIMIT_EDITION_SELL_START_H_
#define PACKET_ITEM_LIMIT_EDITION_SELL_START_H_

#include "PacketHeader.h"

class Packet_Item_Limit_Edition_Sell_Start : public PacketHeader
{
public:
    char m_fieldA;   // +0xa
    int m_fieldB;    // +0xb
    Packet_Item_Limit_Edition_Sell_Start() : PacketHeader(0x1004, 0x7ef)
    {
        m_fieldB = 0;
        m_fieldA = 0;
    }
} __attribute__((packed));

#endif  // PACKET_ITEM_LIMIT_EDITION_SELL_START_H_
