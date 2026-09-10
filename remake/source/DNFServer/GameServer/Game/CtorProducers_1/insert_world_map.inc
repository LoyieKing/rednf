bool CWorldMapList::insert_world_map(CWorldMap* worldMap)
{
    // [b28d-46 判词·N/N86] 沿用 b28c-lane25：73 vs 68，map::insert 的 4.1
    // 「临时槽+拷贝」vs 4.4 copy-elision（N86）⇒ 封顶。
    // b28c-lane25 判词 N86：ORIG 4.1 对 map::insert 的 pair/迭代器恒「临时槽+
    // 拷贝」两段，4.4 直构进目标槽（copy-elision）⇒ 73/68，无源码解（patterns N86）。
    // ORIG 0x08364694 逐条（报告 call 标注被 report_resolve 误标，以地址与 C 段为准）：
    //  ① 0x083646a5 `call get_index` → 结果落 -0x2c 作 find 实参；
    //  ② 0x083646c7 `call map::find` 到临时 -0x3c，再 0x083646d2 拷进持久迭代器槽 -0x30；
    //  ③ 0x083646e2 `call map::end` → 持久槽 -0x28；
    //  ④ 0x083646f7 `call _Rb_tree_iterator::operator==` + `test; je 0x0836475f`
    //     ⇒ **真路径 fall-through 是"未命中→插入"**（技巧 17），不是我们的"命中→报错"；
    //  ⑤ 插入分支 0x08364700 **再 call 一次 get_index** 落 -0xc，
    //     0x08364722 make_pair<int,CWorldMap*&>、0x08364737 pair<int const,CWorldMap*>::pair
    //     两级临时、0x08364750 map::insert；
    //  ⑥ 报错分支 0x0836475f **又 call 一次 get_index**（共 3 次）⇒ 不得缓存 idx 局部
    //     （P-H），且 find/end 都要具名持久槽（技巧 30 + 技巧 21 槽序）。
    // P50 复活（b28c C28L29）：ORIG 有独立 `_Rb_tree_iterator::C1Ev`（-0x30 槽）
    // + find 结果经 sret -0x3c 拷入 ⇒ 源形必为**先声明后赋值**；初始化式会被
    // 4.4 折叠成直构（少 C1Ev 与拷贝两段）。
    std::map<int, CWorldMap*>::iterator it;
    it = m_map.find(worldMap->get_index());
    std::map<int, CWorldMap*>::iterator last = m_map.end();
    // [b28d-solo-CtorProducers_1 判词·N93] ORIG it=-0x30（深）/last=-0x28（浅）
    // 为 4.1「先声明深」；4.4 对头声明 it（C1Ev 首用）给浅槽 -0x2c。last 必须保持
    // 初始化式（ORIG end 直构无 C1Ev），无法提前声明 ⇒ 槽方向不可翻（N93 类对象，
    // 标量例外不适用）。方案 A（last 先声明后赋值）实测 73→78 且多一条 C1Ev ⇒ 回退。
    if (it == last)
    {
        m_map.insert(std::make_pair(worldMap->get_index(), worldMap));
        return true;
    }
    LogManager::logFormat(
        1, "data_manager.cpp",
        __PRETTY_FUNCTION__, 0x2be9,
        "Already registed world map index detected : %d",
        worldMap->get_index());
    return false;
