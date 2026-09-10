int CDeathTower::CStage::dropItem(map_item& item)
{
    // ORIG 0x84616a8：idx = m_mapInfo.m_dropItemCnt++；item.m_itemIndex = idx；
    // MapInfo::Add_Item(m_mapInfo, item)（按值拷贝入参）；返回 idx。
    int idx = m_mapInfo.m_dropItemCnt++;
    item.m_itemIndex = idx;
    m_mapInfo.Add_Item(item);
    return idx;
