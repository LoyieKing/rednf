// ---- drop_item(map_item) —— ORIG 0x0830db84（drop_item.md 全文） ----
// GetCurrentMapInfo()->m_dropItemCnt(+0x40) 作 uid 分配器：取旧值、+1 回写，
// 再 MapInfo::Add_Item(map_item)（ORIG 将按值参数复制到栈上传入）。
int CBattle_Field::drop_item(map_item item)
{
    MapInfo* mapInfo = GetCurrentMapInfo();
    int uid = mapInfo->m_dropItemCnt++;
    // ORIG 0x0830dbad：将 uid 写回 item.m_itemIndex（Add_Item 以 m_itemIndex
    // 为 map key，覆盖可保证 key 唯一不冲突）。
    item.m_itemIndex = uid;
    mapInfo->Add_Item(item);
    return uid;
}