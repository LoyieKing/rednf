void CUserCharacInfo::setUpgradeSeparateTrueFlag(char flag)
{
    if (m_selected)
    {
    CUR->m_upgradeSeparateTrue = (unsigned char)flag;
    }
