void CUserCharacInfo::setUpgradeSeparateFalseFlag(char flag)
{
    if (m_selected)
    {
    m_selected->m_upgradeSeparateFalseFlag = (unsigned char)flag;
    }
}