unsigned int CCandidateMatrix::At(unsigned int idx, int& x, int& y)  // ORIG 0x82e590c（恒 false）
{
    if (m_points.size() > idx)
    {
        x = m_points[idx].m_point.m_x;
        y = m_points[idx].m_point.m_y;
    }
    return 0;
}