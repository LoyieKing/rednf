int WarField::CheckPickupItem(int idx, int owner,
                              std::map<int, map_item>::iterator& out)
{
    std::map<int, map_item>::iterator it;
    it = m_mapInfo.m_itemMap.find(idx);
    if (it == m_mapInfo.m_itemMap.end())
    {
        return 0x15;
    }
    int curSec = GlobalData::s_systemTime_.getCurSec();
    if (curSec - it->second.m_createTick < 10)
    {
        if (!(it->second.m_dropIndex == (unsigned short)-1 ||
              it->second.m_dropIndex == owner))
        {
            return 0x1c;
        }
    }
    out = it;
    return 0;
}