int CLuckPoint::GetUserGainItem(int idx)
{
    if (idx < 0 || static_cast<unsigned int>(idx) >= m_points.size())
    {
        return 0;
    }
    const int userSeq = m_points[idx];
    if (userSeq < 0 || static_cast<unsigned int>(userSeq) >= m_users.size())
    {
        return 0;
    }
    return reinterpret_cast<int>(m_users[userSeq]);
