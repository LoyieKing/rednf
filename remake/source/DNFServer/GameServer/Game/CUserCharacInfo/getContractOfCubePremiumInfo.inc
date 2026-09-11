bool CUserCharacInfo::getContractOfCubePremiumInfo(unsigned char& active, unsigned char& count)
{
    if (!m_selected)
        return false;
    *(&active) = CUR->m_cubePremiumContract != 0 ? 1 : 0;
    *(&count) = CUR->m_cubePremiumCount;
    return true;
}