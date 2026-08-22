#ifndef DNF_GAME_MSG_STAT_RANKRES_H_
#define DNF_GAME_MSG_STAT_RANKRES_H_

// df_game_r MSG_STAT_RANKRES —— 唯一声明点（MSG_STAT_RANKRES.cpp 实现；
// Statistics.cpp / SigTypes.h 统一 include 本头）。
// 布局：+0x00..+0x18 七个整型 + +0x1c GameResultType（0x50 字节），总尺寸 0x6c。
// ORIG：
//   MSG_STAT_RANKRES::MSG_STAT_RANKRES()  0x08610798 —— 仅构造 +0x1c 的 m_result
//                                                 （call _ZN14GameResultTypeC1Ev）
//   MSG_STAT_RANKRES::~MSG_STAT_RANKRES() 0x086107ae —— 仅析构 +0x1c 的 m_result
//                                                 （call _ZN14GameResultTypeD1Ev）
// 前置字段（m_a..m_g）由使用方（Statistics::RankResult）在 SendSignal 前填充，ctor 不 init。

#include "GameResultType.h"

class MSG_STAT_RANKRES
{
public:
    MSG_STAT_RANKRES();
    ~MSG_STAT_RANKRES();

    unsigned int m_a;      // +0x00
    unsigned int m_b;      // +0x04
    int m_c;               // +0x08
    int m_d;               // +0x0c
    int m_e;               // +0x10
    int m_f;               // +0x14
    int m_g;               // +0x18
    GameResultType m_result;  // +0x1c
};

#endif  // DNF_GAME_MSG_STAT_RANKRES_H_
