int CBattle_Field::drop_item(map_item item)
{
    MapInfo* mapInfo = GetCurrentMapInfo();
    int uid = mapInfo->m_dropItemCnt++;
    // ORIG 0x0830dbad：将 uid 写回 item.m_itemIndex（Add_Item 以 m_itemIndex
    // 为 map key，覆盖可保证 key 唯一不冲突）。
    item.m_itemIndex = uid;
    mapInfo->Add_Item(item);
    return uid;
