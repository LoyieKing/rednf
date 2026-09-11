void CUserCharacInfo::SetCurCharacExpertJobExp(int exp)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_expertJobExp = exp;
    }
}