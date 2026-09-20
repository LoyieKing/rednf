void CUserCharacInfo::IncVillageAttackCount()
{
    if (m_selected)
    {
        ++m_selected->m_field1242;
        m_selected->m_field1244 = 1;
    }
}