//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HELLPARTY_STATISTIC_ITEM_H
#define PACKET_HELLPARTY_STATISTIC_ITEM_H

#include "PacketHeader.h"

class Packet_HellParty_Statistic_Item : public PacketHeader {
public:
    bool m_hellpartyType;   // +0xa（SendHellPartyItemStatistic param_1；key.m_hellpartyType；DB log_hellparty_value.hellparty_type）
    int m_dungeonIndex;     // +0xb（param_2；key.m_dungeonIndex；DB dungeon_index）
    char m_dungeonDiff;     // +0xf（param_3；key.m_dungeonDiff；DB dungeon_diff）
    char m_partyCount;      // +0x10（param_4；key.m_partyCount；DB party_count）
    char m_hellpartyDiff;   // +0x11（param_5；key.m_hellpartyDiff；DB hellparty_diff）
    int m_data[6];          // +0x12..+0x29（param_6 0x18 字节；接收端 memcpy(0x18) → HellPartyItenmData::m_data；m_count 隐含每次 +1）

    Packet_HellParty_Statistic_Item() : PacketHeader(0xc31, 0x2a){};
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_HellParty_Statistic_Item, 0x2a);

#endif  // PACKET_HELLPARTY_STATISTIC_ITEM_H
