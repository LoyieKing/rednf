void CUserCharacInfo::incCurCharacLevel()
{
    enableSaveCharacInfo();
    if (m_selected)
    {
    ++CUR->m_level;
    }
