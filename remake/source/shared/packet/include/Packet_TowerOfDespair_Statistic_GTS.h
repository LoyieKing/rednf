//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_TOWEROFDESPAIR_STATISTIC_GTS_H
#define PACKET_TOWEROFDESPAIR_STATISTIC_GTS_H

#include "PacketHeader.h"

class Packet_TowerOfDespair_Statistic_GTS : public PacketHeader {
public:
    Packet_TowerOfDespair_Statistic_GTS() : PacketHeader(0x271c, 0x17),
        m_enterFlag(0), m_accId(0)
    {

    };
    unsigned int m_padA;          // +0xa（第三轮穷举 2026-08-16：_do_after_dungeon_start@834d8ec
                                  //        只写 +0xe/+0x12/+0x13；ClearDungeon@85a9330 只写
                                  //        +0xe/+0x12；Statics AddTowerOfDespairStatistic 只读
                                  //        +0xe/+0x12/+0x13（+0x13 以裸指针读作 accId）；
                                  //        +0xa 无任何写/读点，保留占位）
    int m_layer;                  // +0xe（层数，(0, 0x64]）
    unsigned char m_enterFlag;    // +0x12（_do_after_dungeon_start=1；ClearDungeon=0）
    unsigned int m_accId;         // +0x13（_do_after_dungeon_start: CUser::get_acc_id）
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_TowerOfDespair_Statistic_GTS, 0x17);


#endif //PACKET_TOWEROFDESPAIR_STATISTIC_GTS_H
