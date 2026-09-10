int CMonsterDrop::_getGenItemProbIdx(int value)
{
    unsigned int i = 0;
    for (; m_probsSub.size() > i; ++i)                 // +0x24 工作副本表
    {
        stGenItemProb_t& e = m_probsSub[i];
        if (e.v[0] <= value && e.v[1] >= value)
            break;
    }
    if (m_probsSub.size() <= i)
        return -1;
    return i;
