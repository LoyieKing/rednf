//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DEATHTOWER_STATISTIC_PLAYDATA_PARTY_H
#define PACKET_DEATHTOWER_STATISTIC_PLAYDATA_PARTY_H

#include "PacketHeader.h"

class Packet_DeathTower_Statistic_Playdata_Party : public PacketHeader {
public:
    char m_deathTowerType;  // +0xa（SendDeathTowerPlayInfo param_2；key.m_deathTowerType；DB log_deathtower_playdata_party.type）
    char m_partyCount;      // +0xb（param_5；key.m_partyCount；DB party_count）
    int m_clearStage;       // +0xc（param_4 clearStage；PlayDataPartyStatistic.m_data[0] → avgClearCount 分子）

    Packet_DeathTower_Statistic_Playdata_Party(): PacketHeader(0xc3f,0x10) {};
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_DeathTower_Statistic_Playdata_Party, 0x10);


#endif //PACKET_DEATHTOWER_STATISTIC_PLAYDATA_PARTY_H
