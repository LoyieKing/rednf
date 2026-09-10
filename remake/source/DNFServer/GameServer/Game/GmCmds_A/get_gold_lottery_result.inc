void CBoosterTest::get_gold_lottery_result(CItem const* item,
                                           std::map<int, int>& result)
{
    Inven_Item lotto;
    for (int i = 0; i < m_count; ++i)
    {
        // [b28d-solo-GmCmds_A] P167/P0：ORIG 直读 m_itemList(+0xc)；key 为
        // `lea lotto+7` 直传（无 itemId 具名槽）。
        G_CDataManager()->m_itemList->get_lotto_item(
            const_cast<CItem*>(item)->get_index(),
            reinterpret_cast<char*>(&lotto));
        std::map<int, int>::iterator pos = result.find(lotto.m_addInfo2);
        // [b28d-solo-GmCmds_A] P220：ORIG 用 ne。
        if (pos != result.end())
        {
            ++pos->second;   // ORIG 单次 ptEv（同 getJarResult）
        }
        else
        {
            // [b28d-solo-2] P315/P234：ORIG +0x0cc 直接把 `lea lotto+7` 作第一实参
            // （无右值临时拷贝）且调 `make_pair<int,int>(int&&,int&&)`
            // （_ZSt9make_pairIiiE…OS2_OS5_）⇒ 用 static_cast<int&&> 显式右值化，
            // 避免 4.4 为值拷贝多发 mov 槽往返。
            result.insert(std::make_pair(static_cast<int&&>(lotto.m_addInfo2),
                                         static_cast<int&&>(1)));
        }
    }
