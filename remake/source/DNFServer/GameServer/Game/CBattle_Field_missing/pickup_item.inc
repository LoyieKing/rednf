// ================= CBattle_Field::pickup_item(int) — ORIG 0x0830daee =================
// 在当前地图的 itemMap 中查找 uid；找到则 erase 并返回 0，否则返回 0x15(21)。
int CBattle_Field::pickup_item(int uid)
{
    MapInfo* mi = GetCurrentMapInfo();
    // ORIG 0x0830daee：iterator 先默认构造（C1Ev），find() 结果经独立 sret
    // 临时 -0x1c 中转再拷贝入 it@-0x14（P28 先声明后赋值；GetFieldMob 同法实证）。
    std::map<int, map_item>::iterator it;
    it = mi->m_itemMap.find(uid);
    if (it == mi->m_itemMap.end())
        return 0x15;
    mi->m_itemMap.erase(it);
    return 0;
}