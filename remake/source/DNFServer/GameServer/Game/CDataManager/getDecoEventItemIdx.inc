int CDataManager::getDecoEventItemIdx()
{
    std::vector<int>::iterator it = m_seriaRoomDeco.m_eventItemIdx.begin();
    if (it != m_seriaRoomDeco.m_eventItemIdx.end())
    {
        return *it;
    }
    return 0xffffffff;
}