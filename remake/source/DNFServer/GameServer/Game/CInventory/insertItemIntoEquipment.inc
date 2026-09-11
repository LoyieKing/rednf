int CInventory::insertItemIntoEquipment(Inven_Item item, eItemAddReason reason, bool bLog, bool b)
{
    // b28d-solo-CInventory：88/88，if/else 极性已改（ORIG je 落 logFormat 尾块）⇒ 仅余 N7 间接 call 目标 %eax↔%edx 封顶。
    // b28d-37 [N7 封顶，ORIG 0x84ff8??]：间接 call 目标寄存器 %eax↔%edx 互换（ORIG `mov (%eax),%edx;...call *%edx` vs 4.4 `mov (%eax),%eax;...call *%eax`）；P230 换 4.4.6 后端可翻，本波不改路由 ⇒ 封顶。88/88，语义闭合。
    CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (pItem == 0)
    {
        return -1;
    }
    if (m_invenItem[pItem->GetItemType()].m_addInfo != 0)
    {
        return -2;
    }
    m_invenItem[pItem->GetItemType()].setCopy(item);
    if (bLog)
    {
        if (m_pParent != 0)
        {
            m_pParent->m_historyLog.ItemAdd(
                (INVEN_TYPE)1, 1, 1, item, reason);
        }
        else
        {
            LogManager::logFormat(
                1, "inventory.cpp",
                __PRETTY_FUNCTION__,
                0x14f9, "History ERROR, m_pParent NULL, ItemAdd ");
        }
    }
    return pItem->GetItemType();
}