// ---- get_field_item(int, map_item&) —— ORIG 0x0830d9c0（get_field_item.md） ----
// 当前地图 m_itemMap(+0x24) 查找 uid；命中则 21 dword 复制到出参并返回 1。
// ORIG 形态同 is_last_field_item：迭代器先默认构造（C1Ev @+0x0f）、find 经
// sret 临时槽（-0x2c）赋值（P28），op-> 一次 + rep movsl 21 dword 整体复制
// （P83/N78 4.1 内联形态）。
bool CBattle_Field::get_field_item(int uid, map_item& out)
{
    std::map<int, map_item>::iterator it;
    it = GetCurrentMapInfo()->m_itemMap.find(uid);
    if (it == GetCurrentMapInfo()->m_itemMap.end())
        return false;
    out = it->second;
    return true;
}