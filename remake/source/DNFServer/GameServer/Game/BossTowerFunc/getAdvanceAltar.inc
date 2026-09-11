// ----------------------------------------------------------------------------
// ORIG T 0x08655262 _ZN5CUser15getAdvanceAltarEv
// m_advanceAltarIdx(+0x8d012 short)<0 → 0；否则 G_CGameManager()->getAdvanceAltar(idx)。
// ----------------------------------------------------------------------------
advancealtar::StageControl* CUser::getAdvanceAltar()
{
    // ORIG 0x08655262：同 getBossTower——三目返回形。
    return ((short)m_advanceAltarIdx < 0)
               ? 0
               : G_CGameManager()->getAdvanceAltar(
                     static_cast<int>(static_cast<short>(m_advanceAltarIdx)));
}