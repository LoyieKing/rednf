// df_game_r Game/ BuffInfo 实现（批3：从 GameStubs.cpp 迁移）。
#include "BuffInfo.h"
#include <string.h>

BuffInfo::BuffInfo()
{
    reset();
}

void BuffInfo::reset()
{
    memset(m_pad, 0, sizeof(m_pad));
}