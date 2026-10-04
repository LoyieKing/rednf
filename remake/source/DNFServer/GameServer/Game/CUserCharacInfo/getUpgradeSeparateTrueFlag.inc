char CUserCharacInfo::getUpgradeSeparateTrueFlag()
{
    return m_selected ? (char)m_selected->m_upgradeSeparateTrueFlag : 0;
}