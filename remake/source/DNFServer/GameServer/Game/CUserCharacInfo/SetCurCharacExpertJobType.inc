void CUserCharacInfo::SetCurCharacExpertJobType(int type)
{
    if (m_selected)
    {
    enableSaveCharacInfo();
    CUR->m_expertJobType = type;
    }
}