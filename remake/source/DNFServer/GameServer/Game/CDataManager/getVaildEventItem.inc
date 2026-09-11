bool CDataManager::getVaildEventItem(int idx)
{
    (void)idx;
    std::vector<int>::iterator it = m_seriaRoomDeco.m_eventItemIdx.begin();
    if (it != m_seriaRoomDeco.m_eventItemIdx.end())
    {
        return true;
    }
    return false;
}