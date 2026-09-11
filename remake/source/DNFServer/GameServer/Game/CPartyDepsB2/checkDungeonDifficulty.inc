// ============================================================================
// CParty::checkDungeonDifficulty —— ORIG 0x085be932
// ============================================================================
bool CParty::checkDungeonDifficulty(char difficulty, CDungeon const* dungeon)
{
    if (dungeon == 0)
        return false;
    if (difficulty == 0)
        return false;
    if (m_manager->IsGameMasterMode())                  // +0x74（ORIG 直取不判空）
        return false;
    if (dungeon->isTournamentDungeon())
        return false;
    if (getManager()->getMoveSpace() == (UserSpace::T)1)
        return false;
    if (dungeon->isRiskDungeon())
        return false;
    return true;
}