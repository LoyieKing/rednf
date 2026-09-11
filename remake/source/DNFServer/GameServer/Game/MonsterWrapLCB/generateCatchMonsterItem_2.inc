// ----------------------------------------------------------------------------
// ORIG T 0x08349f4b _Z24generateCatchMonsterItemRKSt6vectorI13CatchItemInfoSaIS0_EERS_I10Inven_ItemSaIS5_EE
// （size 0x11a）：CatchItemInfo 三成员均 int 存储，第 3 成员由 flds 按 float
// 位型消费，故以 reinterpret 视图承载。
//   count(m_field4)==0 → 跳过；
//   rate(m_field8 视 float)==0.0f → 跳过（fnstsw+sahf 仅判 ZF：NaN 继续走）；
//   r = get_rand_int(0x64)；(double)rate > (double)r 才生成；
//   Inven_Item{}；item.m_addInfo = m_field0；create_item((T)1,item,1)；
//   push_back count 次（signed setg 比较）。
// [b28d-L14 P167/N90] ORIG 0x08598d2c；已改 m_itemList 直取（消 EXTRA getItemList）；
// 残差=12B 结构体按值拷贝形（N90 族）。
// [F2e-Singles2 C05③ 2026-09-10] D08 `||` 合并两守卫（m_field4==0 ‖ rate==0.0f）
//   **实测零效果**：我方汇编改前/改后逐条相同、our_insns 93=93、岛数不变
//   （3×`jmp+nop` 仍在）⇒ 已回滚（属 4.4 块布局/A28 族，非「多守卫同目标」可治）。
void generateCatchMonsterItem(const std::vector<CatchItemInfo>& items,
                              std::vector<Inven_Item>& out)
{
    for (std::vector<CatchItemInfo>::const_iterator it = items.begin();
         it != items.end(); ++it)
    {
        const CatchItemInfo entry = *it;                    // [0x8349f86 de 三字段拷贝]
        Inven_Item item;                                    // [0x8349f9c C1 -0x61：先于三守卫]
        const float rate =
            *reinterpret_cast<const float*>(&entry.m_field8);

        // [laneE1 2026-09-11 P296/P223 复现] 旧形三条 `continue` 使 4.4 在环尾发
        //   3×`jmp <tail>; nop` 中继岛；ORIG 只有唯一 tail 0x0834a03e，三守卫各
        //   一条 `je tail` 直达（0x0834fa7/0x8349fbe+0x8349fc0/0x8349fe3）
        //   ⇒ 改**正向嵌套 if 包裹**（P296 已在 CheckDungeonEventStoryPause 实证；
        //   旧 D08 `||` 合并实测零效果，见上方判词）。
        if (entry.m_field4 != 0)                            // [0x8349fa7 test/je tail]
        {
            if (rate != 0.0f)                               // [0x8349fb2 fucompp: jp/je tail]
            {
                const int r = get_rand_int(0x64);           // [0x8349fc2 $0x64]
                if (static_cast<double>(rate) > static_cast<double>(r))
                {                                           // [0x8349fd1 seta; je tail]
                    item.m_addInfo = entry.m_field0;        // [0x8349fe5 写 +0x02 视图]
                    G_CDataManager()->m_itemList->create_item(
                        (ENUM_ITEM_CREATE_TYPE)1, item, 1); // [0x8349ffd create_item(1,.,1)]
                    for (int j = 0; j < entry.m_field4; ++j) // [0x834a031 setg signed]
                        out.push_back(item);
                }
            }
        }
    }
}