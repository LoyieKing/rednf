unsigned char CUserCharacInfo::isSavedPremiumCubeInfo() const
{
    if (!m_selected)
        return 0;
    return CUR->m_cubePremiumSaved;
}