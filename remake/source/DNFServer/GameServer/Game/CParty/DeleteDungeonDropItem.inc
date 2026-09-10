int CParty::DeleteDungeonDropItem(int idx)
{   // ORIG W 0x0860b62a：转发 CBattle_Field::pickup_item 并透传其返回值
    // （processRouting 以 `0 < 返回值` 判定删除失败）。
    return reinterpret_cast<CBattle_Field*>(&m_battleField)->pickup_item(idx);
