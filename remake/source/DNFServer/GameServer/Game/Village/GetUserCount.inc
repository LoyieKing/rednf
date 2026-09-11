int Village::GetUserCount(int area) const
{
    if (area < 0 || area >= m_areaCount)
        return 0;
    return m_pAreas[area].GetUserCount();
}