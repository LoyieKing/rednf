int CSpecialMonsterDrop::_getGenItemProbIdx(int value)
{
    // [b28d-solo-WongWorkDrop_B] AE：44=44（同 Hell 版；残差仅重定位标签）。
    // [b28d-38] ORIG 形同 CMonsterDrop_Hell 版（两份物理独立块）。
    unsigned int i = 0;
    for (; m_probsSub.size() > i; ++i)
    {
        stGenItemProb_t& e = m_probsSub[i];
        if (e.v[0] <= value && e.v[1] >= value)
            break;
    }
    if (m_probsSub.size() <= i)
        return -1;
    return i;
