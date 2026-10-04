void CUserCharacInfo::setUpgradeSeparateTrueFlag(char flag)
{
    if (m_selected)
    {
    m_selected->m_upgradeSeparateTrueFlag = (unsigned char)flag;
    }
}