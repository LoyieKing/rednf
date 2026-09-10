int GameWorld::GetUserCount(int vill, int area) const
{
    if (vill < 0 || vill >= MAX_VILLAGE_NUM)
    {
        return 0;
    }
    return m_pVill[vill].GetUserCount(area);
