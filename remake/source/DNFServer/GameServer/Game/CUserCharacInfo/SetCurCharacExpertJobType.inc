void CUserCharacInfo::SetCurCharacExpertJobType(int type)
{
    if (m_selected)
    {
    enableSaveCharacInfo();
    m_selected->m_fielde7d = type;
    }
}