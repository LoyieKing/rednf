void CGuildAgitArea::SetGuildAgitAreaDomain(int start, int end)
{
    for (int area = start; area < end; ++area)
    {
        m_areaStack.push(area);
    }
