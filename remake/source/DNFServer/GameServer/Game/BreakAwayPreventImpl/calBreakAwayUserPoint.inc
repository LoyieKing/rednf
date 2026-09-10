bool CBreakAwayPreventSystem::calBreakAwayUserPoint()
{
    int lowCnt = 0;
    unsigned int highCnt = 0;

    for (unsigned int idx = 0xf; idx <= 0x1c; ++idx)
    {
        if (m_tail[idx] == 0x31)
        {
            if (idx > 0x15)
                ++highCnt;
            else
                ++lowCnt;
        }
    }
    if (highCnt != 1)
        return false;
    return true;
