void CBoosterTest::getBoosterResult(CItem const* item,
                                    std::map<int, int>& result)
{
    // [b28d-solo-GmCmds_A] P0：ORIG +0x008 `mov 0xc(%ebp),%eax; mov %eax,-0x10`
    // 具名副本槽（getBoosterItem receiver 从 -0x10 读），声明先于 items。
    CStackableItem const* stackable = static_cast<CStackableItem const*>(item);
    std::vector<stStackableBoosterItemInfo_t> items;
    for (int i = 0; i < m_count; ++i)
    {
        stackable->getBoosterItem(items);
        // [b28d-solo-GmCmds_A] P167/P28：ORIG 先 begin()→C1<iterator> 转换构造，
        // 再 end()→C1 转换构造，两个 const_iterator 具名后 while 循环；写成
        // `it != items.end()` 会选混合 ne（p167 EXTRA）。
        std::vector<stStackableBoosterItemInfo_t>::const_iterator it = items.begin();
        std::vector<stStackableBoosterItemInfo_t>::const_iterator end = items.end();
        while (it != end)
        {
            // [b28d-solo-GmCmds_A] P167：ORIG 用 operator*（deEv），非 ptEv。
            // [round5-GmCmdsA] P157/P0（无槽律）：ORIG 全程**无 itemId 具名槽**
            // ——0x084afa60 deEv 后直存 -0x38 仅作 find 实参临时，0x084afad4 处
            // 再 deEv 一次存 -0x14 作 make_pair 实参；具名 itemId 会多占一槽
            // 并连带整组槽深偏 4（旧读数 items -0x48 vs ORIG -0x4c）。
            // ⇒ 两个使用点各自重读成员。
            std::map<int, int>::iterator pos = result.find((*it).m_itemIdx);
            if (pos != result.end())
            {
                ++pos->second;
            }
            else
            {
                // [b28d-47] P61：实参右值化 ⇒ 实例化 make_pair<int,int>（值对），
                // 左值 itemId 会实例化 make_pair<int&,int>（P167 EXTRA）。
                // [b28d-solo-GmCmds_A] ORIG 此处重读 (*it).m_itemIdx（第二个 deEv）。
                result.insert(std::make_pair(static_cast<int>((*it).m_itemIdx), 1));
            }
            ++it;
        }
        items.clear();
    }
