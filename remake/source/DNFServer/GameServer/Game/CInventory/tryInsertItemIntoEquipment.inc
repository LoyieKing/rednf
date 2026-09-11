int CInventory::tryInsertItemIntoEquipment(Inven_Item item) const
{
    // b28c-L2 [N7 封顶]：ORIG 0x0850a(报告) 两处 CItem::GetItemType 虚调用为
    // `mov (%eax),%edx; mov -0xc(%ebp),%eax; mov %eax,(%esp); call *%edx`
    // （目标指针落 %edx、实参落 %eax）；4.4-O0 对间接 call 目标恒 %eax（范式库 N7，
    // 附 4.1 按上下文用 ecx/edx），临时/引用/限定名/语句表达式均不可翻（N7 已 8 件
    // 实证）。语义（虚槽 + 单实参 + 调用序）与 ORIG 全等，仅剩寄存器名差 ⇒ 保持
    // 可读直调形，ndiff 8 封顶（fresh 复验：orig 44 / our 44）。
    CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (pItem == 0)
    {
        return -1;
    }
    if (m_invenItem[pItem->GetItemType()].m_addInfo != 0)
    {
        return -2;
    }
    return pItem->GetItemType();
}