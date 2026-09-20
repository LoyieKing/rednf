bool CUserCharacInfo::initContractOfCubePremiumInfo(bool active, unsigned char count)
{
    if (!m_selected)
        return false;
    m_selected->m_field14D0 = 0;
    m_selected->m_field14CE = active;
    m_selected->m_field14CF = count;
    return true;
}