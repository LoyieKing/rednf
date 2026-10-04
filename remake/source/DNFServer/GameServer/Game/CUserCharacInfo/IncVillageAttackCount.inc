void CUserCharacInfo::IncVillageAttackCount()
{
    if (m_selected)
    {
        ++m_selected->m_villageAttackCount;
        m_selected->m_villageAttackDBUpdate = 1;
    }
}