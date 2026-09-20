bool CUserCharacInfo::getContractOfCubePremiumInfo(unsigned char& active, unsigned char& count)
{
    if (!m_selected)
        return false;
    *(&active) = m_selected->m_field14CE != 0 ? 1 : 0;
    *(&count) = m_selected->m_field14CF;
    return true;
}