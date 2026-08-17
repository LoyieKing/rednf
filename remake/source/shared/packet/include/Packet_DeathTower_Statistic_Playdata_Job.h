//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DEATHTOWER_STATISTIC_PLAYDATA_JOB_H
#define PACKET_DEATHTOWER_STATISTIC_PLAYDATA_JOB_H

#include "PacketHeader.h"

class Packet_DeathTower_Statistic_Playdata_Job : public PacketHeader {
public:
    char m_deathTowerType;      // +0xa（SendDeathTowerPlayInfo param_2；key.m_deathTowerType；DB log_deathtower_playdata_job.type）
    unsigned short m_level;     // +0xb（stDeathTowerRecordMemberInfo_t[+0x14]；key.m_level；DB level）
    unsigned int m_characJob;   // +0xd（member[+0x15]；key.m_characJob；DB charac_job）
    char m_characGrow;          // +0x11（member[+0x16]；key.m_characGrow；DB charac_grow）
    int m_clearStage;           // +0x12（param_4 clearStage；ValueStatistic.m_data[0] → avgClearCount 分子）

    Packet_DeathTower_Statistic_Playdata_Job(): PacketHeader(0xc3d,0x16) {};
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_DeathTower_Statistic_Playdata_Job, 0x16);


#endif //PACKET_DEATHTOWER_STATISTIC_PLAYDATA_JOB_H
