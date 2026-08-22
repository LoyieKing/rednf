// df_game_r Game/ 迁移 TU —— MSG_STAT_RANKRES 构造/析构（2026-08-21）。
// 权威声明见 MSG_STAT_RANKRES.h（Statistics.cpp / SigTypes.h 统一 include）。
// ORIG：
//   MSG_STAT_RANKRES::MSG_STAT_RANKRES()  0x08610798 —— 仅构造 +0x1c 的 m_result
//                                                 （call _ZN14GameResultTypeC1Ev）
//   MSG_STAT_RANKRES::~MSG_STAT_RANKRES() 0x086107ae —— 仅析构 +0x1c 的 m_result
//                                                 （call _ZN14GameResultTypeD1Ev）
// 前置字段（m_a..m_g）由使用方（Statistics::RankResult）在 SendSignal 前填充，ctor 不 init。
#include "MSG_STAT_RANKRES.h"
#include <new>

MSG_STAT_RANKRES::MSG_STAT_RANKRES()
{
    new (&m_result) GameResultType();
}

MSG_STAT_RANKRES::~MSG_STAT_RANKRES()
{
    m_result.~GameResultType();
}
