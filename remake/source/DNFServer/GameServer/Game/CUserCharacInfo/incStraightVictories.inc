void CUserCharacInfo::incStraightVictories()
{
    if (m_selected)
    {
    ++m_selected->m_straightVictories;
    }
}