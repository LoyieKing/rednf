void CBoosterTest::getJarResult(CItem const* item, std::map<int, int>& result)
{
    Inven_Item lotto;
    for (int i = 0; i < m_count; ++i)
    {
        // [b28d-47] P0：ORIG 0x084afbe8 `call get_index; mov %eax,%ebx`（无槽）⇒
        // 实参内联，禁具名 idx（4.4 落 -0xc 槽并使帧/槽整体漂移）。
        // [b28d-solo-GmCmds_A] P167：ORIG 直读 CDataManager::m_itemList(+0xc)
        // （`call G_CDataManager; mov 0xc(%eax),%eax`），getItemList() 是 EXTRA call。
        G_CDataManager()->m_itemList->get_lotto_item(
            const_cast<CItem*>(item)->get_index(),
            reinterpret_cast<char*>(&lotto));
        int itemId = lotto.m_addInfo;
        std::map<int, int>::iterator pos = result.find(itemId);
        // [b28d-solo-GmCmds_A] P220：ORIG 用 ne（`call ne; test; je <insert>`）。
        if (pos != result.end())
        {
            ++pos->second;   // ORIG 单次 ptEv + mov/add/mov（`= pos->second + 1` 4.4 双 ptEv）
        }
        else
        {
            // [b28d-solo-GmCmds_A] P61：ORIG 调 make_pair<int,int>（IiiE 值对）；
            // 左值实参生成 make_pair<int&,int>（IRiiE，p167 EXTRA）。
            // [b28d-solo-GmCmds_A] ORIG +0x0b1 从 lotto+2 重读（`mov -0x73(%ebp),%eax`），
            // 非从 itemId 槽（-0x34）读。
            result.insert(std::make_pair(static_cast<int>(lotto.m_addInfo), 1));
        }
    }
