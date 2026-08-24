// df_game_r Game/ CDescending 实现（批3：从 GameStubs.cpp 迁移）。
// ORIG 0x85df910：比较器 operator()(int,int)，setg → 返回 (a > b)（降序谓词）。
#include "CDescending.h"

bool CDescending::operator()(int a, int b)
{
    return a > b;
}