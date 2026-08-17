//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_SERVER_MATCH_DATA_H
#define PACKET_SERVER_MATCH_DATA_H

#include "PacketHeader.h"

class Packet_Server_Match_data : public PacketHeader {
public:
    Packet_Server_Match_data() : PacketHeader(0x1b67, 0xd),
        m_serverGroup(0), m_win(0), m_lose(0)
    {

    };
    char m_serverGroup;   // +0xa（pvp_reward/GiveOuterPenalty：CUser::GetServerGroup）
    char m_win;   // +0xb（pvp_reward：房间胜标志（add_pvp_result 同源）非 0 时置 1）
    char m_lose;   // +0xc（pvp_reward：胜标志为 0 时置 1；GiveOuterPenalty 恒置 1）
} __attribute__((packed));

#endif  // PACKET_SERVER_MATCH_DATA_H
