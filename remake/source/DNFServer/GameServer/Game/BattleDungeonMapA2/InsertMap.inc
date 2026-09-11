// ---- InsertMap(int, int, CMap*) —— ORIG 0x0834b988（InsertMap.md 全文） ----
// 按 CMap::get_map_type() 选择容器：0 → m_map1a4[idx]；2 → m_dummyMap4a4[idx]
// 并置 m_624=1；其余 → m_dummyMap324[idx]。direction 位 1/2/4/8 各插入一条
// pair(方向键, CMap*)。三段结构按 ORIG 原样展开。
// [b28d-solo-BattleDungeonMapA2] NEAR 封顶（N82）：347=347 助记符序列全等、
// P167 调用点差集 CLEAR；本波把 12 组 pair 构造实参改为 PATH_DIRECTION 枚举
// （ORIG mangled `C1I14PATH_DIRECTIONRS2_EEOT_OT0_`，权威头 CDungeon.h 新增
// 全局 enum PATH_DIRECTION 定义）后调用点符号全等。残余 102 行全为 4.1
// 不复用槽（各分支独立 key@-0xbc/-0xac/…）vs 4.4 生命周期复用
// （-0x68/-0x60/…）+ call/jmp 目标显示差；槽策略跨版本不可翻。
// [b28e 复核 347=347，p167 CLEAR] 残差 = 12 个 if 块的 pair/iter/temp 槽排布：
//   ORIG 每块 0x10 字节**连续交错**（块内浅→深 temp(4)/pair(8)/iter(4)，
//   如 +0x8a 段 int@-0xbc/pair@-0xc4/iter@-0xc8，块间步长 0x10）；4.4 按**类型
//   分区**（pair 区步长 8、iter 区、temp 区各自连续）⇒ N82附「4.4 聚合对象
//   优先占低槽、标量分区排布」型，声明/具名均不可改分区规则 ⇒ 封顶。
void CDungeon::InsertMap(int idx, int direction, CMap* map)
{
    if (map->get_map_type() == 0)
    {
        if ((direction & 1) != 0)
        {
            std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)1, map);
            m_map1a4[idx].insert(value);
        }
        if ((direction & 2) != 0)
        {
            std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)2, map);
            m_map1a4[idx].insert(value);
        }
        if ((direction & 4) != 0)
        {
            std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)4, map);
            m_map1a4[idx].insert(value);
        }
        if ((direction & 8) != 0)
        {
            std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)8, map);
            m_map1a4[idx].insert(value);
        }
    }
    else
    {
        if (map->get_map_type() == 2)
        {
            if ((direction & 1) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)1, map);
                m_dummyMap4a4[idx].insert(value);
            }
            if ((direction & 2) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)2, map);
                m_dummyMap4a4[idx].insert(value);
            }
            if ((direction & 4) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)4, map);
                m_dummyMap4a4[idx].insert(value);
            }
            if ((direction & 8) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)8, map);
                m_dummyMap4a4[idx].insert(value);
            }
            m_hasDummyMap = 1;                         // +0x624（byte 写）
        }
        else
        {
            if ((direction & 1) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)1, map);
                m_dummyMap324[idx].insert(value);
            }
            if ((direction & 2) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)2, map);
                m_dummyMap324[idx].insert(value);
            }
            if ((direction & 4) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)4, map);
                m_dummyMap324[idx].insert(value);
            }
            if ((direction & 8) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)8, map);
                m_dummyMap324[idx].insert(value);
            }
        }
    }
}