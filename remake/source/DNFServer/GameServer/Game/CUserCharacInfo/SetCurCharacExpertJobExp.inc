void CUserCharacInfo::SetCurCharacExpertJobExp(int exp)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_expertJobExp = exp;
    }
}