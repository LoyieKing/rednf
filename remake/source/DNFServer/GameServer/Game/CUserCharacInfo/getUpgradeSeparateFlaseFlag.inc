char CUserCharacInfo::getUpgradeSeparateFlaseFlag()
{
    return m_selected ? (char)m_selected->m_upgradeSeparateFalseFlag : 0;
}