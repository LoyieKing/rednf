//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PCROOMPLAYTIMEREWARD_H
#define PACKET_PCROOMPLAYTIMEREWARD_H

#include "PacketHeader.h"

class Packet_PcRoomPlayTimeReward : public PacketHeader {
public:
    char m_serverGroup;  // +0xa  processEventPcRoomPlayTime: CUser::GetServerGroup（非整合 PvP 用环境 +0x378）
    int m_accId;         // +0xb  CUser::get_acc_id
    int m_characNo;      // +0xf  CUserCharacInfo::getCurCharacNo

    Packet_PcRoomPlayTimeReward() : PacketHeader(0x27ec, 0x13),
        m_serverGroup(0), m_accId(0), m_characNo(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_PcRoomPlayTimeReward, 0x13);

#endif  // PACKET_PCROOMPLAYTIMEREWARD_H
