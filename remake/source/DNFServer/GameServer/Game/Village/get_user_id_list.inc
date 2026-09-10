void Village::get_user_id_list(
    int area,
    std::vector<unsigned short, std::allocator<unsigned short> >& list)
{
    if (area < 0 || area >= m_areaCount)
    {
        LogManager::logFormat(
            1, "world.cpp",
            "void Village::get_user_id_list(int, std::vector<short unsigned "
            "int, std::allocator<short unsigned int> >&)",
            0x36a, "area(%d) < 0 || area >= m_iAreaCount(%d)", area,
            m_areaCount);
    }
    else
    {
        m_pAreas[area].get_user_id_list(list);
    }
