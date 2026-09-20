void CUserCharacInfo::decCurCharacLevel()
{
    enableSaveCharacInfo();
    if (m_selected)
    {
    --m_selected->m_level;
    }
}