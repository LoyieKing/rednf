int CSchoolArea::GetAreaIndex()
{
    if (m_areaStack.empty())
    {
        return -1;
    }
    int areaIdx = m_areaStack.top();
    m_areaStack.pop();
    return areaIdx;
