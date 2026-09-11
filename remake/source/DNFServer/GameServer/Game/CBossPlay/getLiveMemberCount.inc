// ============================================================================
// getLiveMemberCount（ORIG 0x08149466）
// ============================================================================
int CBossPlay::getLiveMemberCount()
{
    int count = 0;
    for (int i = 0; i <= 3; ++i)
    {
        if (m_memberAlive[i])
            ++count;
    }
    return count;
}
