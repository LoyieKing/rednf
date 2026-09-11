bool CDungeonDataManager::InsertDungeonKey(DungeonKey& key)
{
    std::map<DungeonKey, CDungeonNode>::iterator it = m_dungeonMap.find(key);
    if (it != m_dungeonMap.end())
        return false;
    // [b28e-N5 · N93/N82附 翻正] node 写成表达式临时 CDungeonNode()。CDungeonNode 有用户提供
    //   的默认 ctor（CDungeonNode.h:123 `CDungeonNode()` W 0x08486ae6）⇒ value-init 不额外
    //   零初始化，与具名 `CDungeonNode node;` 语义等价；析构仍在 insert 语句末，与 ORIG
    //   0x0848577e→0x084857f5 的 D1 序（pair<const> → pair → node）一致；ORIG 无前置 rep stos。
    //   机理：4.4 把「具名对象」放深区、把「表达式临时」放浅区（与 4.1 相反）⇒ 具名 node 形
    //   得到 node=-0x1e0（其余 5 槽整体前移，与 ORIG 镜像，NEAR）；临时形一次对齐全部 6 槽：
    //   node=-0x98 / make_pair=-0x134 / pair<const>=-0x1d0 / insert.ret=-0x1d8 / end=-0x1dc /
    //   it=-0x1e0（ORIG 0x084856ac 逐槽实证；探针 /tmp/r5/n5/mgr）。
    //   make_pair 第二参为右值（OS_ 形），实例仍为 <DungeonKey&, CDungeonNode>；p167 仅余
    //   C1/C2、D1/D2 同址别名（P93 豁免）。
    m_dungeonMap.insert(std::make_pair(key, CDungeonNode()));
    return true;
}