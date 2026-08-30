// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG（nm_28 仅 ctor）+ 定义提取自 Game/Store.cpp:40-47。
// 调用方 CDeathTowerValueStatistic::SendDeathTowerValueStatistic 先 memset 后填
// m_a/m_b/m_field/m_c；本头保留 cpp 原始字段布局（plain POD，无 ctor 符号之外
// 证据），接入时由宿主迁除 cpp 内重复段。
//
// 2026-08-30 更新：ORIG C1 0x0860ffc2 实证为 PacketHeader(0xc3b,0x15) 构造
// （SendDeathTowerValueStatistic 0x0860f342 显式调用 C1），故改为与
// shared/packet/include/Packet_DeathTower_Statistic_Value.h 一致的多态版本。
#ifndef DNF_GAME_PACKET_DEATHTOWER_STATISTIC_VALUE_H_
#define DNF_GAME_PACKET_DEATHTOWER_STATISTIC_VALUE_H_

#include "PacketHeader.h"

class Packet_DeathTower_Statistic_Value : public PacketHeader
{
public:
    unsigned char m_type;       // +0xa（发送 SendDeathTowerValueStatistic param_3；接收 key.m_type；DB log_deathtower_value.type）
    unsigned short m_level;     // +0xb（param_4；key.m_level；DB level）
    unsigned int m_counterIdx;  // +0xd（param_2：字段 10 时映射 3..6，否则原值；key.m_counterIdx；DB 计数器下标）
    int m_value;                // +0x11（param_5；ValueStatistic.m_data[0]；DBMW STDeathTowerValueWireItem.m_value）

    Packet_DeathTower_Statistic_Value() : PacketHeader(0xc3b, 0x15) {}
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_DEATHTOWER_STATISTIC_VALUE_H_
