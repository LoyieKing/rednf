void CCandidateMatrix::RemoveCandidatePoint(std::vector<MapArea>* areas) // ORIG 0x82e5f36
{
    std::vector<CandidatePoint>::iterator it = m_points.begin();
    while (it != m_points.end())
    {
        if (!::IsIncludeVirtualMoveAreas(areas, (*it).m_point.m_x,
                                       (*it).m_point.m_y))   // ORIG 用 operator*（de）
        {
            it = m_points.erase(it);
            continue;
        }
        ++it;
    }
