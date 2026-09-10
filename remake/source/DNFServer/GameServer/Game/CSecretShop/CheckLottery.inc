void CSecretShop::CheckLottery()
{
    // [b28d-solo-CSecretShop] p167 定位（ORIG 56 calls / 旧实现 48）：
    // ① MISSING `_Hashtable_iterator::ptEv`×4 ⇒ ORIG 每处 `it->second` 重读（无
    //    `CDungeon* dungeon` 缓存，P183）；② MISSING `__normal_iterator::deEv`×2/
    //    `ppEv`/`ne`/`begin`/`end` + EXTRA `vector::ixEj`×2 ⇒ 内层遍历是**迭代器**
    //    形（`(*lt).m_itemIdx`，P167 星点判据）；③ EXTRA `_Rb_tree_iterator::eq` ⇒
    //    正极性 `c != counts.end()`（P220）；④ ORIG make_pair 实例 `_ZSt9make_pairIiiE`
    //    （右值版）⇒ 实参右值化。
    // 判词·N（本轮复核）：p167 CLEAR（56=56，仅 C1/C2、D1/D2 别名伪差）；
    // gap -2（304 vs 306）= 4.4 对 make_pair<int,int>(int&&,int&&) 的 xvalue
    // 实参 materialize 读值临时（ORIG 0x08a81e8b 直传 &(*lt).m_itemIdx）；已把
    // `(int)` cast 改 P234 形 `static_cast<int&&>`（符号与 ORIG 一致，仍 +2 条
    // ⇒ 跨版本 codegen 封顶）。尾部 setcc-fold（ORIG `setb;test;jne` vs 4.4
    // 直跳+常量岛，P197④ 镜像负例）与 N82/N89 失真并存；语义完成。
    // ORIG 0x85fad20：统计工具 —— 对 CDataManager 地下城表中每个地下城，
    // 以 dungeonIdx=0x3ea(1002)、level=get_index()、price=get_standard_level()
    // 抽取 10000 次 LotteryItems，统计各商品出现次数，并把百分比写入
    // secretshop_lottery.txt。
    std::vector<SALE_INFO> list;
    std::map<int, int> counts;
    FILE* fp = fopen("secretshop_lottery.txt", "wt");
    if (fp == 0)
    {
        return;
    }
    // [b28d-solo-CSecretShop] P0/P183：ORIG 0x085fad68（begin 前）与 0x085fb0d5
    // （end 前）各 fresh 调一次 G_CDataManager ⇒ 无 `hash_map& dungeons` 缓存局部。
    for (__gnu_cxx::hash_map<int, CDungeon*>::iterator it =
             G_CDataManager()->m_dungeonList->m_dungeonMap.begin();
         it != G_CDataManager()->m_dungeonList->m_dungeonMap.end(); ++it)
    {
        counts.clear();
        for (int i = 0; i < 10000; ++i)
        {
            LotteryItems(list, 0x3ea, it->second->get_index(),
                         it->second->get_standard_level());
            for (std::vector<SALE_INFO>::iterator lt = list.begin();
                 lt != list.end(); ++lt)
            {
                std::map<int, int>::iterator c =
                    counts.find((*lt).m_itemIdx);
                if (c != counts.end())
                {
                    ++c->second;
                }
                else
                {
                    // [b28d-solo-CSecretShop] ORIG 0x085fae8b `call deEv; mov
                    // %eax,%edx; …; mov %edx,0x4(%esp); call make_pair<int,int>
                    // (int&&,int&&)`——实参即 `&(*lt).m_itemIdx`（+0 字段，无读值
                    // 临时）。`(int)` cast 会先读值落临时（+2 条）；P234 形
                    // `static_cast<int&&>` 直接绑定右值引用，符号与 ORIG 一致。
                    counts.insert(std::make_pair(
                        static_cast<int&&>((*lt).m_itemIdx), 1));
                }
            }
            list.clear();
        }
        fprintf(fp, "Dungeon : %s(idx:%d, lev:%d)\n",
                it->second->GetDungeonName(), it->second->get_index(),
                it->second->get_standard_level());
        int total = 0;
        for (std::map<int, int>::iterator c = counts.begin();
             c != counts.end(); ++c)
        {
            total += c->second;
        }
        for (std::map<int, int>::iterator c = counts.begin();
             c != counts.end(); ++c)
        {
            fprintf(fp, "- %d(%d, %.2f%%)\n", c->first, c->second,
                    100.0 * ((double)c->second / (double)total));
        }
        fputc('\n', fp);
    }
    fclose(fp);
