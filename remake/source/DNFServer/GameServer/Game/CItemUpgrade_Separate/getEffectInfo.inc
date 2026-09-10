OutLineEFFECTInfoTemp GenuineDamageUpgradeTable::getEffectInfo(int key)
{
    static OutLineEFFECTInfoTemp temp;

    if (m_maxLevel < key)
    {
        return temp;
    }

    std::map<int, OutLineEFFECTInfoTemp>::iterator it =
        m_effects.find(key);
    std::map<int, OutLineEFFECTInfoTemp>::const_iterator cit(it);
    if (cit != m_effects.end())
    {
        return cit->second;
    }
    return temp;
