void CUserCharacInfo::VillageAttack_NotDBUpdate()
{
    if (m_selected)
    {
    CUR->m_villageAttackDBUpdate = 0;
    }
}