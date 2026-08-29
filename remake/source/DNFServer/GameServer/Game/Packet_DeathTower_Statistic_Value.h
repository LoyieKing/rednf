// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG（nm_28 仅 ctor）+ 定义提取自 Game/Store.cpp:40-47。
// 调用方 CDeathTowerValueStatistic::SendDeathTowerValueStatistic 先 memset 后填
// m_a/m_b/m_field/m_c；本头保留 cpp 原始字段布局（plain POD，无 ctor 符号之外
// 证据），接入时由宿主迁除 cpp 内重复段。
#ifndef DNF_GAME_PACKET_DEATHTOWER_STATISTIC_VALUE_H_
#define DNF_GAME_PACKET_DEATHTOWER_STATISTIC_VALUE_H_

struct Packet_DeathTower_Statistic_Value
{
    char m_pad0[0xa];
    char m_a;
    short m_b;
    int m_field;
    int m_c;
};

#endif  // DNF_GAME_PACKET_DEATHTOWER_STATISTIC_VALUE_H_
