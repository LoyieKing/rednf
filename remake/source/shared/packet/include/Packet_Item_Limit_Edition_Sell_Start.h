#ifndef PACKET_ITEM_LIMIT_EDITION_SELL_START_H_
#define PACKET_ITEM_LIMIT_EDITION_SELL_START_H_

#include "PacketHeader.h"

class Packet_Item_Limit_Edition_Sell_Start : public PacketHeader
{
public:
    Packet_Item_Limit_Edition_Sell_Start() : PacketHeader(0x1004, 0x7ef)
    {
        *(int*)((char*)this + 0xb) = 0;
        *(char*)((char*)this + 0xa) = 0;
    }
} __attribute__((packed));

#endif  // PACKET_ITEM_LIMIT_EDITION_SELL_START_H_
