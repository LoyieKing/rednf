//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DUNGEON_STATISTIC_PARTY_CHARAC_H
#define PACKET_DUNGEON_STATISTIC_PARTY_CHARAC_H

#include "PacketHeader.h"

class Packet_Dungeon_Statistic_Party_Charac : public PacketHeader {
public:
    unsigned short m_channelNo;           // +0xa（SendDungeonStatisticPartyCharac get_channel_no；接收端 key.m_channelNo 恒置 0 不读）
    int m_dungeonIndex;                   // +0xc（get_dungeon_index；key.m_dungeonIndex）
    char m_dungeonDiff;                   // +0x10（get_dungeon_diff；key.m_dungeonDiff）
    unsigned char m_dungeonStandardLevel; // +0x11（this+0x354；key.m_dungeonStandardLevel）
    unsigned char m_success;              // +0x12（param_1；key.m_success）
    unsigned int m_characJob;             // +0x13（get_charac_job；key.m_characJob）
    char m_characGrow;                    // +0x17（getCurCharacGrowType；key.m_characGrow）
    char m_partyUserCount;                // +0x18（user+0x8e054；key.m_partyUserCount）
    int m_data[10];                       // +0x19..+0x40（user+0x8e034..0x8e050/0x8e064/0x8e068；→ PartyCharacStatistic.m_data[0..9]）
    short m_field41;                      // +0x41（user+0x8e058 short；→ m_data[10]；
                                          //         不能并入 m_data[]：m_data[11] 线上缺失，
                                          //         +0x41 为 short 无法作为 int[] 元素，独立命名）
    int m_field43;                        // +0x43（this+0x183c ushort 读出后恒置 0；→ m_data[12]；
                                          //         m_data[11] 不传输）

    Packet_Dungeon_Statistic_Party_Charac(): PacketHeader(0xc39,0x47) {};
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Dungeon_Statistic_Party_Charac, 0x47);


#endif //PACKET_DUNGEON_STATISTIC_PARTY_CHARAC_H
