//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_COLLECTITEMSGM_H
#define PACKET_COLLECTITEMSGM_H

#include "PacketHeader.h"

class Packet_CollectItemsGm : public PacketHeader {
public:
    unsigned char m_serverGroup;  // +0xa（monitor onCollectItemsGm：m_sg = Get_ServerGroup()；DBMW 视图 m_serverInfo）
    int m_totalCount;             // +0xb（DBMW updateCollectItemsGm SQL total_count；oldGmRoutine "tg" 参数1）
    int m_curCount;               // +0xf（DBMW SQL cur_count；oldGmRoutine "tg" 参数2）
    int m_fullTime;               // +0x13（DBMW SQL full_time=from_unixtime；回应包作 m_changeFlag）
    Packet_CollectItemsGm() : PacketHeader(0x27eb, 0x17),
        m_serverGroup(0), m_totalCount(0), m_curCount(0), m_fullTime(0)
    {
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_CollectItemsGm, 0x17);

#endif  // PACKET_COLLECTITEMSGM_H
