void CUserCharacInfo::setCurCharacPremiumFatigue(unsigned short value)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_premiumFatigue = value;
    }
}