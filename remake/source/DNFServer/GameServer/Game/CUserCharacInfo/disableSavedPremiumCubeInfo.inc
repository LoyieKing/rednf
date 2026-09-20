void CUserCharacInfo::disableSavedPremiumCubeInfo()
{
    if (!m_selected)
        return;
    m_selected->m_field14D0 = 0;
}