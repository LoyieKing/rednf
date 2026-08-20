// df_game_r Game/ CCharacterView 实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "CUser.h"

void CCharacterView::reset()
{
    disableSaveCharacView();
}

void CCharacterView::enableSaveCharacView()
{
    m_bSaveCharacView = true;
}

bool CCharacterView::isSaveCharacView()
{
    return m_bSaveCharacView;
}

void CCharacterView::disableSaveCharacView()
{
    m_bSaveCharacView = false;
}
