void Village::send_to_area(int area, PacketGuard& pkt)
{
    if (area < 0 || area >= m_areaCount)
    {
        LogManager::logFormat(
            1, "world.cpp", __PRETTY_FUNCTION__,
            0x360, "area(%d) < 0 || area >= m_iAreaCount(%d)", area,
            m_areaCount);
    }
    else
    {
        m_pAreas[area].send_to_all(pkt);
    }
