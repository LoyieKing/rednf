int CItemAmplifier::getPurifyMaterialItemCount(int type) const
{
    if (type >= 0 && type <= 1)
    {
        return m_script.m_purifyMatCount[type];
    }
    return 0;
