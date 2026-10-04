void CUserCharacInfo::SetOpencoin(char value)
{
    if (m_selected)
    {
        if ((char)m_selected->m_opencoin != value)
        {
            enableSaveCharacStat();
            m_selected->m_opencoin = (unsigned char)value;
        }
    }
}