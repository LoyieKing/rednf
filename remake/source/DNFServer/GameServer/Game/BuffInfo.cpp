// df_game_r Game/ BuffInfo 实现（批3：从 GameStubs.cpp 迁移）。
#include "BuffInfo.h"

BuffInfo::BuffInfo()
{
    m_exp = 1;
    m_gold = 1;
    m_drop = 1;
}

void BuffInfo::reset()
{
    m_exp = 1;
    m_gold = 1;
    m_drop = 1;
}
