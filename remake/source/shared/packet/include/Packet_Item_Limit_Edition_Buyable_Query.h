//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_ITEM_LIMIT_EDITION_BUYABLE_QUERY_H
#define PACKET_ITEM_LIMIT_EDITION_BUYABLE_QUERY_H

#include "PacketHeader.h"

class Packet_Item_Limit_Edition_Buyable_Query : public PacketHeader {
public:
    unsigned int m_seq;       // +0xa（回显到 GameServer）
    unsigned int m_ipgno;     // +0xe
    char m_payload[0x1ca];    // +0x12
    Packet_Item_Limit_Edition_Buyable_Query(): PacketHeader(0x1009,0x1dc) {};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Item_Limit_Edition_Buyable_Query, 0x1dc);


#endif //PACKET_ITEM_LIMIT_EDITION_BUYABLE_QUERY_H
