void CUserCharacInfo::IncVillageAttackCount()
{
    if (m_selected)
    {
        ++CUR->m_revengeCount;
        CUR->m_villageAttackDBUpdate = 1;
    }
