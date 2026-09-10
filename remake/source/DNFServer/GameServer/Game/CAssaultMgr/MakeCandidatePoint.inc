void CCandidateMatrix::MakeCandidatePoint(const Point2d& a, const Point2d& b) // ORIG 0x82e5ca8
{
    // b28d-L3：ORIG 0x82e5ca8 头部两条 `movl $0,-0x10(%ebp); movl $0,-0xc(%ebp)`
    // = 全头部声明 i/j 各带初值（P212），第一循环 init 覆写 `i=1` ⇒ 双初始化形。
    int i = 0;
    int j = 0;
    _AddCandidatePoint(a, a, b, 10);
    for (i = 1; i <= 7; ++i)
    {
        _AddCandidatePoint(Point2d(a.m_x, a.m_y + i * 10), a, b, 10);
        _AddCandidatePoint(Point2d(a.m_x, a.m_y - i * 10), a, b, 10);
    }
    for (j = 0; ::GetMaxCandidateMatirxRow() > j; ++j)
    {
        for (i = 1; i <= 7; ++i)
        {
            _AddCandidatePoint(Point2d(m_points[j].m_point.m_x + i * 10,
                                       m_points[j].m_point.m_y), a, b, 0);
            _AddCandidatePoint(Point2d(m_points[j].m_point.m_x - i * 10,
                                       m_points[j].m_point.m_y), a, b, 0);
        }
    }
