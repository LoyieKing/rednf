//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_CHANGE_POWER_WAR_POINT_H
#define PACKET_GUILD_CHANGE_POWER_WAR_POINT_H

#include <vector>

#include "PacketHeader.h"
#include <string.h>

class Packet_Guild_Change_Power_War_Point : public PacketHeader {
public:
    Packet_Guild_Change_Power_War_Point() : PacketHeader(0x6a4, 0x2d),
        m_powerSide(0), m_winnerPoint(0), m_loserPoint(0)
    {
    memset((char*)this + 0xb, 0, 0x10);
    memset((char*)this + 0x1b, 0, 0x10);
    };
    void GetUserList(std::vector<unsigned int>& list);
    unsigned char m_powerSide;       // +0xa  _SendPowerWarPointToGuildServer: CUser::getPowerSide
    unsigned int m_winnerCharNos[4]; // +0xb..+0x1a  auStack_62[0..3] 胜利方 charac_no（get_charac_no）
    unsigned int m_loserCharNos[4];  // +0x1b..+0x2a  auStack_62[4..7] 失败方 charac_no
    unsigned char m_winnerPoint;     // +0x2b  win pp（local_42 = multiple*param_5 + _VictoryBonus）
    unsigned char m_loserPoint;      // +0x2c  lose pp（local_41 = multiple*(winCnt-winAliveCnt)）
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Guild_Change_Power_War_Point, 0x2d);

#endif //PACKET_GUILD_CHANGE_POWER_WAR_POINT_H
