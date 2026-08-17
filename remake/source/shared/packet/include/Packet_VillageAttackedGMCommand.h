//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_VILLAGEATTACKEDGMCOMMAND_H
#define PACKET_VILLAGEATTACKEDGMCOMMAND_H

#include "PacketHeader.h"

class Packet_VillageAttackedGMCommand : public PacketHeader {
public:
    unsigned int m_uid;      // +0xa（SendVillageAttackedGMCommand/execute：CUser::GetUID）
    unsigned int m_charNo;   // +0xe（CUser::get_charac_no）
    int m_field12;           // +0x12（第三轮穷举 2026-08-16：仅 game_master::CVillageAttackedStartCmd
                             //      ::execute@84aa144 写 *(this+8)（命令对象成员无类定义）；
                             //      SendVillageAttackedGMCommand@820bb68 不写；monitor
                             //      OnVillageAttackedGMCommand 只读 m_charNo；townatk GM 命令
                             //      （oldGmRoutine@820bf2c）success/fail/end→m_command 1/2/3；
                             //      无 m_field12 消费端证据，保留占位）
    int m_command;           // +0x16（SendVillageAttackedGMCommand param_2：townatk success=1/fail=2/end=3；
                             //      execute 置 0）
    Packet_VillageAttackedGMCommand() : PacketHeader(0x1779, 0x1a),
        m_uid(0), m_charNo(0)
    {

    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_VillageAttackedGMCommand, 0x1a);


#endif //PACKET_VILLAGEATTACKEDGMCOMMAND_H
