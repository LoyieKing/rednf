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
    char m_payload[0x1ca];    // +0x12（客户端提交的可买数量/物品记录，monitor 原样回拷）
    Packet_Item_Limit_Edition_Buyable_Query();   // ORIG _ZN39Packet_Item_Limit_Edition_Buyable_QueryC1Ev @ 0x0822c064
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Item_Limit_Edition_Buyable_Query, 0x1dc);


#endif //PACKET_ITEM_LIMIT_EDITION_BUYABLE_QUERY_H
