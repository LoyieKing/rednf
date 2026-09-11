void CUserCharacInfo::SetOpencoin(char value)
{
    if (m_selected)
    {
        if ((char)CUR->m_opencoin != value)
        {
            enableSaveCharacStat();
            CUR->m_opencoin = (unsigned char)value;
        }
    }
}