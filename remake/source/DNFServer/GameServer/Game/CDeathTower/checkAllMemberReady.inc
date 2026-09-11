bool CDeathTower::CPlayData::checkAllMemberReady(int memberCnt)
{
    // ORIG 0x08461e7e：m_allReady 已置位直接 true；否则统计 m_memberReady，
    // 计满 memberCnt 时置 m_allReady=1 并返回 true（循环固定 4 槽）。
    if (m_allReady != 0)
        return true;
    int readyCnt = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (m_memberReady[i])
        {
            ++readyCnt;
            if (readyCnt == memberCnt)
            {
                m_allReady = 1;
                return true;
            }
        }
    }
    return false;
}