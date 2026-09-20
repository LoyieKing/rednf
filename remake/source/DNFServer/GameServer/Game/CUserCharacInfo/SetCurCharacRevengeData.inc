void CUserCharacInfo::SetCurCharacRevengeData(unsigned char count, unsigned char ticket)
{
    if (m_selected)
    {
    m_selected->m_field1242 = count;
    m_selected->m_field1243 = ticket;
    }
}