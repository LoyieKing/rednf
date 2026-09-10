void CUserCharacInfo::disableSavedPremiumCubeInfo()
{
    if (!m_selected)
        return;
    CUR->m_cubePremiumSaved = 0;
