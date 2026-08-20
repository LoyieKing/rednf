// df_game_r Game/ Sanicova 命名空间类实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "CUser.h"

namespace Sanicova
{

int CPad::getCancelCnt()
{
    return 0;
}

void CPad::setCancelCnt(int)
{
}

int CPad::getRequestState()
{
    return 0;
}

bool CPad::isActivate() const
{
    return false;
}

int CPad::getFailCnt()
{
    return 0;
}

void CPad::setFailCnt(int)
{
}

}  // namespace Sanicova
