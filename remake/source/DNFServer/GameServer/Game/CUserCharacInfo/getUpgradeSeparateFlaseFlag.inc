char CUserCharacInfo::getUpgradeSeparateFlaseFlag()
{
    return m_selected ? (char)CUR->m_upgradeSeparateFalse : 0;
}