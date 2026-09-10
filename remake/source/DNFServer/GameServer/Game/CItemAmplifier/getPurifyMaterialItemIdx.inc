int CItemAmplifier::getPurifyMaterialItemIdx(int type) const
{
    if (type >= 0 && type <= 1)
    {
        return m_script.m_purifyMatItem[type];
    }
    return 0;
