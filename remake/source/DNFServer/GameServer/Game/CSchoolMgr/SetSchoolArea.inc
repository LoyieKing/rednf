void CSchoolArea::SetSchoolArea(int start, int end)
{
    for (int area = start; area < end; ++area)
    {
        m_areaStack.push(area);
    }
