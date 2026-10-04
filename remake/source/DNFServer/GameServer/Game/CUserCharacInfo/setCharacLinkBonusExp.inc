void CUserCharacInfo::setCharacLinkBonusExp(unsigned int exp)
{
    if (m_selected)
    {
    m_selected->m_characLinkBonusExp = exp;
    }
}