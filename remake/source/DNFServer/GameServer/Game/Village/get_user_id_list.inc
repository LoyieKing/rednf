void Village::get_user_id_list(
    int area,
    std::vector<unsigned short, std::allocator<unsigned short> >& list)
{
    if (area < 0 || area >= m_areaCount)
    {
        LogManager::logFormat(
            1, "world.cpp", __PRETTY_FUNCTION__,
            0x36a, "area(%d) < 0 || area >= m_iAreaCount(%d)", area,
            m_areaCount);
    }
    else
    {
        m_pAreas[area].get_user_id_list(list);
    }
}