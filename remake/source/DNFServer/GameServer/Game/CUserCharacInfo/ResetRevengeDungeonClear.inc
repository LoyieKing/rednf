void CUserCharacInfo::ResetRevengeDungeonClear()
{
    if (m_selected)
    {
    m_selected->m_revengeDungeonCount = 0;
    m_selected->m_villageAttackCount = 0;
    }
}