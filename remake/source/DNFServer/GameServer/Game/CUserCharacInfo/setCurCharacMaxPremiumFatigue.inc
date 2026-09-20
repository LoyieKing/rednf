void CUserCharacInfo::setCurCharacMaxPremiumFatigue(unsigned short value)
{
    if (m_selected)
    {
    enableSaveCharacEtc();
    m_selected->m_field79 = value;
    }
}