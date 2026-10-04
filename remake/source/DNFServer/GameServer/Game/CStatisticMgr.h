#ifndef GAME_CSTATISTICMGR_H
#define GAME_CSTATISTICMGR_H

// ============================================================================
// CStatisticMgr — 统计容器全局单例管理器（GlobalData::s_statisticMgr @0941f764）。
// ORIG C1/W 0x082aa89e（ctor 空体、两个统计子对象 map 成员自构造）。
// 布局证据：gain_exp_sp ORIG 0x0866a3fe / 报告 gain_exp_sp.md 调用
//   CLevelDungeonPlayStatistic::IncreaseLevelDungeonPlay(s_statisticMgr, level, count)
// —— s_statisticMgr 首地址即被当作 CLevelDungeonPlayStatistic this 使用，
// 故该子对象位于 +0x00。
// owner：W8_CU_G.cpp 建立；其余 TU 引用请 include 本头，勿重复建模。
// ============================================================================
#include "CLevelDungeonPlayStatistic.h"
#include "CDungeonExpStatistic.h"

class CStatisticMgr
{
public:
    CLevelDungeonPlayStatistic m_levelDungeonPlay;   // +0x00
    CDungeonExpStatistic m_dungeonExp;              // +0x18
};

#endif  // GAME_CSTATISTICMGR_H
