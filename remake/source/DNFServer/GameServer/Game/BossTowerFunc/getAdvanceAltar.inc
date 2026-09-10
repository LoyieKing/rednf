advancealtar::StageControl* CUser::getAdvanceAltar()
{
    // ORIG 0x08655262：同 getBossTower——三目返回形。
    return ((short)m_advanceAltarIdx < 0)
               ? 0
               : G_CGameManager()->getAdvanceAltar(
                     static_cast<int>(static_cast<short>(m_advanceAltarIdx)));
