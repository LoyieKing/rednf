//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_ITEM_LIMIT_EDITION_BUYABLE_QUERY_H
#define PACKET_ITEM_LIMIT_EDITION_BUYABLE_QUERY_H

#include "PacketHeader.h"

class Packet_Item_Limit_Edition_Buyable_Query : public PacketHeader {
public:
    Packet_Item_Limit_Edition_Buyable_Query(): PacketHeader(0x1009,0x1dc) {};
};


#endif //PACKET_ITEM_LIMIT_EDITION_BUYABLE_QUERY_H
