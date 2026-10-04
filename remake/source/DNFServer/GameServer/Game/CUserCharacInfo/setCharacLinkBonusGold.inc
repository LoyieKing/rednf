void CUserCharacInfo::setCharacLinkBonusGold(unsigned int gold)
{
    if (m_selected)
    {
    m_selected->m_characLinkBonusGold = gold;
    }
}