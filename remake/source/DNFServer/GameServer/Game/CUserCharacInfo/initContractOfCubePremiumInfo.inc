bool CUserCharacInfo::initContractOfCubePremiumInfo(bool active, unsigned char count)
{
    if (!m_selected)
        return false;
    CUR->m_cubePremiumSaved = 0;
    CUR->m_cubePremiumContract = active;
    CUR->m_cubePremiumCount = count;
    return true;
