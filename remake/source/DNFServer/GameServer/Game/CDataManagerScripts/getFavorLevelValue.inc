unsigned short CNPCScriptList::getFavorLevelValue(unsigned int level) const
{
    if (level > 3)
        return getMaxFavorValue();
    if (level == 0)
        return 0;
    return m_npcCommon.m_favorExp[level];
