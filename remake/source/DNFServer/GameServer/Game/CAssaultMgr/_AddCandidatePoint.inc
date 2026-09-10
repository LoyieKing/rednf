void CCandidateMatrix::_AddCandidatePoint(const Point2d& from1,
                                          const Point2d& from2,
                                          const Point2d& relTo,
                                          int weight)              // ORIG 0x82e5894
{
    CandidatePoint cp;
    cp.m_point = from1;                        // point ← from1
    cp.m_priority = weight;
    cp.m_priority += from1 - relTo;            // 曼哈顿 op-
    cp.m_priority -= from2 - from1;
    m_points.push_back(cp);
