//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_REQUEST_GUILD_BOARD_WRITE_H
#define PACKET_GUILD_REQUEST_GUILD_BOARD_WRITE_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Guild_Request_Guild_Board_Write : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa（SendGuildBoardWrite param_6 = CUserCharacInfo::get_charac_guildkey）
    unsigned int m_boardNo;     // +0xe（SendGuildBoardWrite param_2 = CUser::get_acc_id → 实为 accId；旧名保留因 .cpp 引用未同步）
    unsigned int m_charNo;      // +0x12（SendGuildBoardWrite param_3 = CUserCharacInfo::getCurCharacNo）
    unsigned char m_job;        // +0x16（SendGuildBoardWrite param_4 = CUserCharacInfo::get_charac_job）
    unsigned char m_growType;   // +0x17（SendGuildBoardWrite param_5 = CUserCharacInfo::getCurCharacGrowType）
    char m_info[0x78];          // +0x18（公告内容；SendGuildBoardWrite param_1，memcpy 0x78）
    Packet_Guild_Request_Guild_Board_Write() : PacketHeader(0x232d, 0x90),
        m_guildKey(0), m_boardNo(0), m_charNo(0), m_job(255), m_growType(255)
    {
    memset(m_info, 0, sizeof(m_info));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Guild_Request_Guild_Board_Write, 0x90);


#endif //PACKET_GUILD_REQUEST_GUILD_BOARD_WRITE_H
