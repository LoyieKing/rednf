void CCompound_ExtreamDun_Item::ForceResultItemRule_Setting(        // ORIG 0x832d784
    int idx, bool flag, int value)
{
    m_data.m_forceRules[idx] = forceRule_ResultItem(flag, value);
