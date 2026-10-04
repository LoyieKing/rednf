unsigned char CUserCharacInfo::GetCurVillageAttackCount() const
{
    return m_selected ? m_selected->m_villageAttackCount : 0;
}