bool CSecurityCard::isCertFlagOn()
{
    if (m_certFlag31 == 1)
        return true;
    return false;
