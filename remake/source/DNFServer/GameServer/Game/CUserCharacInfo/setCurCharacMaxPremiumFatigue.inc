void CUserCharacInfo::setCurCharacMaxPremiumFatigue(unsigned short value)
{
    if (m_selected)
    {
    enableSaveCharacEtc();
    CUR->m_maxPremiumFatigue = value;
    }
