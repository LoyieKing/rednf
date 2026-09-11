bool GameWorld::check_valid_area(int vill, int area)
{
    if (vill < 1 || m_iVillCount < vill)
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x4a2,
                              "[GameWorld::check_valid_area] invalid village : %d, m_iVillCount : %d",
                              vill, m_iVillCount);
        return false;
    }
    if (m_pVill[vill].m_areaCount == 0)
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x4aa,
                              "[GameWorld::check_valid_area] invalid area count : %d",
                              m_pVill[vill].m_areaCount);
        return false;
    }
    if (area < 0 || m_pVill[vill].m_areaCount <= area)
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x4b2,
                              "[GameWorld::check_valid_area] invalid area : %d", area);
        return false;
    }
    return true;
}