void CUserCharacInfo::setUpgradeSeparateFalseFlag(char flag)
{
    if (m_selected)
    {
    m_selected->m_field1274 = (unsigned char)flag;
    }
}