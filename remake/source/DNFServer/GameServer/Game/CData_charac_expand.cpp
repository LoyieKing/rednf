// df_game_r Game/ charac_expand::CData 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 声明以 CBoosterGage.h 为准。
#include "CBoosterGage.h"

namespace charac_expand
{

CData::~CData()
{
}

void CData::alter()
{
    m_bIsSave = 1;
}

bool CData::isSave() const
{
    return m_bIsSave != 0;
}

bool CData::saveData(CUser* pUser)
{
    bool result = _saveData(pUser);
    m_bIsSave = 0;
    return result;
}

void CData::reset()
{
    m_bIsSave = 0;
    _reset();
}

}  // namespace charac_expand
