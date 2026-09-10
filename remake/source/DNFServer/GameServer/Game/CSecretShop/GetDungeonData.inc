int CSecretShopStatistic::GetDungeonData(int npcIdx, int dungeonIdx)
{
    // [b28d-solo-CSecretShop] P183/P123：ORIG 0x085fd147 每处访问都重算
    // `pos*0x18 + this`（无 `map& m` 缓存局部）；insert 返回值被丢弃、随后
    // **再次 find** 取迭代器（ORIG 0x085fd1f7 后 0x085fd230 第二次 call find，
    // 旧实现用 insert().first 少 1 次 find/1 组地址重算）；SECRET_SHOP_STATISTIC_DATA
    // ctor（0x085fd45a）已清 5 个 int ⇒ 只写 m_data[0]（旧实现多写 4 条 movl）。
    // 判词·AE（本轮修复）：`std::make_pair` 改显式 `std::pair<int,T>(dungeonIdx,
    // data)`（ORIG 0x085fd1f7 直接调 pair 模板 ctor，无 make_pair sret）⇒ 消除
    // 调用后 `sub $0x4,%esp`，101→100，gap 0。
    int pos = GetNpcPos(npcIdx);
    std::map<int, SECRET_SHOP_STATISTIC_DATA>::iterator it =
        m_pos[pos].find(dungeonIdx);
    if (it != m_pos[pos].end())
        return (int)&it->second;
    SECRET_SHOP_STATISTIC_DATA data;
    data.m_data[0] = dungeonIdx;
    m_pos[pos].insert(std::pair<int, SECRET_SHOP_STATISTIC_DATA>(dungeonIdx, data));
    it = m_pos[pos].find(dungeonIdx);
    return (int)&it->second;
