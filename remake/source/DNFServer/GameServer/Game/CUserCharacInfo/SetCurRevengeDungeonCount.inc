void CUserCharacInfo::SetCurRevengeDungeonCount(unsigned char count)
{
    if (m_selected)
    {
    m_selected->m_field1243 = count;
    }
}