void CUserCharacInfo::setUpgradeSeparateTrueFlag(char flag)
{
    if (m_selected)
    {
    m_selected->m_field1273 = (unsigned char)flag;
    }
}