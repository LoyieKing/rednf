// ---- calBreakAwayUserPoint（ORIG 0x0831c810）-------------------------------
// UV 名单 idx∈[0xf,0x1c]：'1'(0x31) 计数恰好 1 且落在 (0x15,0x1c] 高位区即命中。
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
}