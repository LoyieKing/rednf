bool CUserCharacInfo::initContractOfCubePremiumInfo(bool active, unsigned char count)
{
    if (!m_selected)
        return false;
    m_selected->m_savedPremiumCubeInfo = 0;
    m_selected->m_cubePremiumActive = active;
    m_selected->m_cubePremiumCount = count;
    return true;
}