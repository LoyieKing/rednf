// ORIG T 0x0830a0e8（报告 docs/class_func_reports/CBattle_Field/check_grid_clear.md）：
// 遍历当前图怪物表，存在 dropCnt==100 且 +0x13 标志为 0 的怪 → false，全部通过 → true。
// [b28d-D43] ORIG 形：mob 在**循环外**默认构造（0x830a0f3 一次 C1Ev，循环内
// operator=）；it 在 for 初始化处直构 begin()；条件端**第二次独立** GetCurrentMapInfo
// （P48 无 CSE）；增量段 `it++`（P28 ppEi）。旧重建缓存 mapInfo/monsters 引用
// （-0x10/-0xc 两槽）且循环内构造 mob ⇒ 78→66。
bool CBattle_Field::check_grid_clear()
{
    map_monster mob;
    for (std::map<int, map_monster>::iterator it =
             GetCurrentMapInfo()->m_monsterMap.begin();
         it != GetCurrentMapInfo()->m_monsterMap.end(); it++)
    {
        mob = it->second;                             // operator= 拷贝
        if (mob.m_dropCnt == 0x64 && mob.m_field13 == 0)
            return false;
    }
    return true;
}