void CUserCharacInfo::disableSavedPremiumCubeInfo()
{
    if (!m_selected)
        return;
    m_selected->m_savedPremiumCubeInfo = 0;
}