// ORIG makeGeneralItemDropList 0x084ae734：普通物品按 itemId 计数；
// itemId==0（金币类）把 Inven_Item.m_addInfo2 累计到键 0。
// b28d-solo-GmCmds_E-2：157=157 **AE**（本轮 near 1→0、ae +1）——P0/P48 条件与
// insert 处重新读成员 + P222 正向包裹 + **find 实参内联**
// （`out.find(it->m_item.m_addInfo)`，P236：ORIG find 实参临时槽 -0x50 比
//  it(-0x58) 浅；具名 `int key` 被 4.4 排到 -0x54 且 end 临时 -0x50 与 ORIG
//  互换）。make_pair 值类别仍保留左值形（ORIG `IiRi` vs 4.4 `Iii`，指令序列
//  identical）。
void CMonsterDropTest::makeGeneralItemDropList(std::list<map_item>& items,
                                               DropStat& out)
{
    for (std::list<map_item>::iterator it = items.begin(); it != items.end(); ++it)
    {
        // P236：ORIG 无 key 具名局部槽（find 实参为内联成员表达式临时
        // `mov 0x12(%eax),%eax; mov %eax,-0x50` ⇒ 临时槽比 it 浅）；
        // 具名 key 会使 4.4 把 key 排到 it 之上（-0x54），ORIG 为 -0x50。
        std::map<int, int>::iterator mit = out.find(it->m_item.m_addInfo);
        // P0/P48：ORIG 条件与 insert 处均重新读成员（`call ptEv; mov 0x12(%eax)`），
        // 不用 key 槽；P222 正向包裹（mit!=end 直落、insert 走尾岛）。
        if (it->m_item.m_addInfo == 0)
        {
            if (mit != out.end())
            {
                mit->second += it->m_item.m_addInfo2;
            }
            else
            {
                out.insert(std::make_pair(
                    static_cast<int>(it->m_item.m_addInfo),
                    it->m_item.m_addInfo2));
            }
        }
        else
        {
            if (mit != out.end())
            {
                mit->second++;
            }
            else
            {
                out.insert(std::make_pair(
                    static_cast<int>(it->m_item.m_addInfo), 1));
            }
        }
    }
}