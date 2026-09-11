void CUserCharacInfo::setCurCharacLevel(int level)
{
    enableSaveCharacInfo();
    if (m_selected)
    {
    CUR->m_level = (unsigned short)level;
    }
}