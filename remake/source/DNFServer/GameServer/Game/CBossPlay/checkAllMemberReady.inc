// ============================================================================
// checkAllMemberReady（ORIG 0x081493fe）
// ============================================================================
bool CBossPlay::checkAllMemberReady(int count)
{
    if (m_allMemberReady)
        return 1;
    int ready = 0;
    for (int i = 0; i <= 3; ++i)
    {
        if (m_memberReady[i])
        {
            ++ready;
            if (ready == count)
            {
                m_allMemberReady = 1;
                return 1;
            }
        }
    }
    return 0;
}
