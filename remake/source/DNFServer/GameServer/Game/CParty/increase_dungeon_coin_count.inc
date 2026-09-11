void CParty::increase_dungeon_coin_count(int param)
{
    if (m_battleField.m_pDungeon == 0)
        return;
    if (m_battleField.m_pDungeon->m_87c == -1)
        return;
    SetUsedCoinCount(param);
}