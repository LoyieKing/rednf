// ORIG get_gold_lottery_result 0x084afd1c：统计金币产出（key=+7 数量，value=1）。
// [b28d-47] verdict=DIFF(routed) [N7/N82] ORIG 0x084afd1c orig/ours=86/87
//   修：idx 实参内联（P0）+ `++pos->second`。残余：寄存器角色/槽差。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=21 [N82/N95] p167 CLEAR(11/11)；
// 全帧槽差 4（lotto -0x6d vs -0x71、pos -0x30 vs -0x34），语义完成结构性封顶。
// [b28d-solo-2] verdict=DIFF：make_pair 第一实参改 `static_cast<int&&>(lotto.m_addInfo2)`
// （P315/P234，ORIG 调 `make_pair<int,int>(int&&,int&&)` 且直接传 `lea lotto+7`）
// 后 orig/ours=87/87；残余仅 4.4 对右值 cast 恒物化临时拷贝（2 条，探针 p16
// f1/f2/f4 均拷贝，4.1 直接传原地址）⇒ N130/P75 封顶。
// [round5-A20 复核] fresh .o 87/86（gap +1）唯一残差=make_pair 首实参：
//   ORIG 0x084afaf0 `lea -0x6d(%ebp),%edx; add $0x7,%edx; mov %edx,0x4(%esp)`（把
//   **字段本身的地址**作 int&& 绑定）vs 4.4.7 对 `static_cast<int&&>(lotto.m_addInfo2)`
//   恒先物化 4B 临时（`mov -0x6a(%ebp),%eax; mov %eax,-0x10(%ebp); lea -0x10,%edx`）
//   ⇒ 4.1 引用直接绑字段、4.4.7 绑临时（`std::move` 形会多发一次 out-of-line call，
//   （P234 禁）⇒ N18a/P75 封顶。
// [L1FreeTop12.S2 2026-09-11] §6.5 复核：本件 ORIG 无串引用、无 PRETTY；§6.5.1 enc 0 命中。
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
}