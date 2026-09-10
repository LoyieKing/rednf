void PvpResultType::AddNewResult(const PvpResultType& other)
{
    if (m_count == 0)
    {
        unsigned savedWinPoint = m_winPoint;
        Set(other);
        // N76 封顶：此 store ORIG 4.1 值先（mov -0x1c,%edx 后 mov 0x8,%eax），
        // 4.4 恒 LHS 地址先载，probe 全试不可翻（负例表 N76）。
        m_winPoint = savedWinPoint;
    }
    else
    {
        m_avg[0] = AverageIncremental<int, long long>(
            (long long)m_avg[0], (long long)other.m_avg[0], (long long)m_count);
        m_avg[1] = AverageIncremental<int, long long>(
            (long long)m_avg[1], (long long)other.m_avg[1], (long long)m_count);
        m_avg[2] = AverageIncremental<int, long long>(
            (long long)m_avg[2], (long long)other.m_avg[2], (long long)m_count);
        m_avg[3] = AverageIncremental<int, long long>(
            (long long)m_avg[3], (long long)other.m_avg[3], (long long)m_count);
        m_avg[4] = AverageIncremental<int, long long>(
            (long long)m_avg[4], (long long)other.m_avg[4], (long long)m_count);
        m_avg[5] = AverageIncremental<int, long long>(
            (long long)m_avg[5], (long long)other.m_avg[5], (long long)m_count);
        m_avg[6] = AverageIncremental<int, long long>(
            (long long)m_avg[6], (long long)other.m_avg[6], (long long)m_count);
        m_avg[7] = AverageIncremental<int, long long>(
            (long long)m_avg[7], (long long)other.m_avg[7], (long long)m_count);
        m_avg[8] = AverageIncremental<int, long long>(
            (long long)m_avg[8], (long long)other.m_avg[8], (long long)m_count);
        m_avg[9] = AverageIncremental<int, long long>(
            (long long)m_avg[9], (long long)other.m_avg[9], (long long)m_count);
        m_avg[10] = AverageIncremental<int, long long>(
            (long long)m_avg[10], (long long)other.m_avg[10], (long long)m_count);
        m_avg[11] = AverageIncremental<int, long long>(
            (long long)m_avg[11], (long long)other.m_avg[11], (long long)m_count);
        m_avg[12] = AverageIncremental<int, long long>(
            (long long)m_avg[12], (long long)other.m_avg[12], (long long)m_count);
        m_avg[13] = AverageIncremental<int, long long>(
            (long long)m_avg[13], (long long)other.m_avg[13], (long long)m_count);
        m_avg[14] = AverageIncremental<int, long long>(
            (long long)m_avg[14], (long long)other.m_avg[14], (long long)m_count);
    }
    ++m_count;
