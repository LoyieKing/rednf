void CUserCharacInfo::setCurCharacLevel(int level)
{
    enableSaveCharacInfo();
    if (m_selected)
    {
    m_selected->m_level = (unsigned short)level;
    }
}