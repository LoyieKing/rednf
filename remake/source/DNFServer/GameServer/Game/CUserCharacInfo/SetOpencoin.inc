void CUserCharacInfo::SetOpencoin(char value)
{
    if (m_selected)
    {
        if ((char)m_selected->m_fielde70 != value)
        {
            enableSaveCharacStat();
            m_selected->m_fielde70 = (unsigned char)value;
        }
    }
}