bool CArtifact::EquipArtifact(const Inven_Item* item)
{
    // ORIG 0x8336f6e：无 item 空指针检查，直接取 item+2（m_addInfo/item idx）。
    // ORIG push %ebx + `mov 0x2(%eax),%eax; mov %eax,%ebx`…`mov %ebx,0x4(%esp)`：
    // idx 值跨 G_CDataManager call 缓存 %ebx（P0 无槽往返）⇒ P197② 语句表达式
    // 实参形令 4.4 升格 callee-saved。
    // P211 同型化：局部直存 CEquipItem*（O0 指针 cast 零成本），使尾 store
    // `m_pItem = pItem` 字段/RHS 同型 ⇒ 4.4 LHS-first = ORIG 0x8336fb9 形。
    CEquipItem* pItem = (CEquipItem*)G_CDataManager()->find_item(
        ({ item->m_addInfo; }));
    if (pItem == 0)
        return false;
    if (pItem->is_stackable())
        return false;
    m_pItem = pItem;
    return true;
}