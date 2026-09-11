// ORIG getRandomboxResult 0x084afe3a：统计 multibox 随机盒产出（pair<Inven_Item,bool>）。
// [b28d-47] verdict=DIFF(routed) [P220/P83] ORIG 0x084afe3a orig/ours=148/123
//   修：P220 正极性 `pos != end`（消 EXTRA eq/MISSING ne）+ P61 make_pair 右值化
//   + `++pos->second`。残余：EXTRA memcpy（我方 memcpy vs ORIG 结构整体赋值
//   rep movsl，N84）+ 槽平移。
// [b28d-solo-GmCmds_A] verdict=NEAR ndiff=10 [N82] p167 CLEAR(16/16)；drops 槽差 4
// （-0x44 vs -0x40），语义完成（结构整体赋值替 memcpy、正极性 ne、make_pair 值对）。
void CBoosterTest::getRandomboxResult(CItem const* item,
                                      std::map<int, int>& result)
{
    Inven_Item dropped;
    std::vector<std::pair<Inven_Item, bool> > drops;
    for (int i = 0; i < m_count; ++i)
    {
        drops.clear();
        const_cast<CStackableItem*>(static_cast<CStackableItem const*>(item))->get_multibox_lottery_item(&drops);
        for (unsigned int j = 0; j < drops.size(); ++j)
        {
            // [b28d-solo-GmCmds_A] N84③：ORIG 为结构整体赋值逐字段浅拷贝
            // （+0x6b 段 16×mov dword + movzbl），memcpy 是 EXTRA call。
            dropped = drops[j].first;
            // [round5-GmCmdsA] P157/P0：ORIG 无 itemId 具名槽（0x084aff79 读
            // dropped+2 直入 find 实参临时、0x084affb3 处再读一次作 make_pair
            // 实参）⇒ 两个使用点各自重读 dropped.m_addInfo。
            std::map<int, int>::iterator pos = result.find(dropped.m_addInfo);
            // [b28d-47] P220 正极性：ORIG 用 `ne`（pos != end 时 ++），旧 `==` 形
            // 产 eq（P167 MISSING ne / EXTRA eq）。
            if (pos != result.end())
            {
                ++pos->second;   // [b28d-47] 单次 ptEv
            }
            else
            {
                result.insert(std::make_pair(static_cast<int>(dropped.m_addInfo), 1));  // [b28d-47] P61 右值化
            }
        }
    }
}