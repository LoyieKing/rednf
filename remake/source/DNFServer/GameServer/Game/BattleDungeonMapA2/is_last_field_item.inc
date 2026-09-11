// ---- is_last_field_item(int) —— ORIG 0x0830da6c（is_last_field_item.md） ----
// 与 get_field_item 同构，但作用于 GetLastMapInfo()；返回 uid 是否仍在场上。
// ORIG 形态（报告 0x830da72-0x830da6b）：迭代器先默认构造（C1Ev @+0x09）、
// find 经 sret 临时槽（-0x1c）再 4 字节赋值到 -0x10（P28「先声明后赋值」）、
// == end 后走 test/je→mov$1 分支岛（P56，非 sete 直折）。
bool CBattle_Field::is_last_field_item(int uid)
{
    std::map<int, map_item>::iterator it;
    it = GetLastMapInfo()->m_itemMap.find(uid);
    if (it == GetLastMapInfo()->m_itemMap.end())
        return false;
    return true;
}