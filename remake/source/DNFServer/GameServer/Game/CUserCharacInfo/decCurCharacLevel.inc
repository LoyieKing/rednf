void CUserCharacInfo::decCurCharacLevel()
{
    enableSaveCharacInfo();
    if (m_selected)
    {
    --CUR->m_level;
    }
}