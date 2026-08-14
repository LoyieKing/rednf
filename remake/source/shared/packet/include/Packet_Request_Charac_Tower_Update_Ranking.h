//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_CHARAC_TOWER_UPDATE_RANKING_H
#define PACKET_REQUEST_CHARAC_TOWER_UPDATE_RANKING_H

#include "PacketHeader.h"

class Packet_Request_Charac_Tower_Update_Ranking : public PacketHeader {
public:
    unsigned int m_charNo;      // +0xa FindUser_CharNo
    unsigned int m_job;         // +0xe
    unsigned int m_floor;       // +0x12
    unsigned int m_score;       // +0x16
    Packet_Request_Charac_Tower_Update_Ranking(): PacketHeader(0x4ce,0x1a) {};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Request_Charac_Tower_Update_Ranking, 0x1a);


#endif //PACKET_REQUEST_CHARAC_TOWER_UPDATE_RANKING_H
