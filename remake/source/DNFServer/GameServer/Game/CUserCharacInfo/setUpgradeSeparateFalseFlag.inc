void CUserCharacInfo::setUpgradeSeparateFalseFlag(char flag)
{
    if (m_selected)
    {
    CUR->m_upgradeSeparateFalse = (unsigned char)flag;
    }
}