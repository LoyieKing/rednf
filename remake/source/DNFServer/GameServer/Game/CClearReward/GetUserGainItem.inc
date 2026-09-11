// ============================================================================
// 全局 ::CLuckPoint 幸运点修正（ORIG T 0x085509da / 0x08550be4）
// ============================================================================
// ORIG T 0x08550e50：m_points[idx]（operator[]）得用户序号；越界返 0，
// 否则 m_users[序号]（ORIG 以 int 承载指针值，调用方 m_ownerId 为 int）。
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
}