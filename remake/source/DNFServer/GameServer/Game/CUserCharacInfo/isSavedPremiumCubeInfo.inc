char CUserCharacInfo::isSavedPremiumCubeInfo() const
{
    if (!m_selected)
        return 0;
    return m_selected->m_savedPremiumCubeInfo;
}