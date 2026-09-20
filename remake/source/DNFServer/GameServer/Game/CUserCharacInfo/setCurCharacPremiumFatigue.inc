void CUserCharacInfo::setCurCharacPremiumFatigue(unsigned short value)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_field77 = value;
    }
}