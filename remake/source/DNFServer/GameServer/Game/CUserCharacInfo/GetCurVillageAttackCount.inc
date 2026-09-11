unsigned char CUserCharacInfo::GetCurVillageAttackCount() const
{
    return m_selected ? CUR->m_revengeCount : 0;
}