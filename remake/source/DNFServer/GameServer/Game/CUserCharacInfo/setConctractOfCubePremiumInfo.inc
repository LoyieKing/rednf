bool CUserCharacInfo::setConctractOfCubePremiumInfo(bool active, unsigned char count)
{
    if (!m_selected)
        return false;
    m_selected->m_field14D0 = 1;
    m_selected->m_field14CE = active;
    m_selected->m_field14CF = count;
    return true;
}