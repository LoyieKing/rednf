void CUserCharacInfo::incCurCharacLevel()
{
    enableSaveCharacInfo();
    if (m_selected)
    {
    ++m_selected->m_level;
    }
}