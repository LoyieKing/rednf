// ---- CheckPrivateStoreDoll（ORIG T 0x85c532c，报告 CheckPrivateStoreDoll.md L15-129 全文）----
// 校验 doll 槽物品可作开店娃娃：来源背包槽存在且为可堆叠、脚本索引有效、数量>0。
// 返回错误码：0x3a 卖家缺失 / 0x16 物品无效 / 0 可用。
// [b28d-24] DIFF 残23: P167 EXTRA Inven_Item::get_add_info×1。ORIG 0x085c532c
int CPrivateStore::CheckPrivateStoreDoll(unsigned short dollSlot)
{
    if (m_pSeller == 0)
        return 0x3a;

    // P52/P183：ORIG 0x085c533a 先把 (short)dollSlot 物化 %ebx 跨 receiver call
    // 保活，receiver 结果直落实参槽，无 inventory/dataMgr 具名缓存槽。
    Inven_Item item = m_pSeller->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1,
                                                                    (int)dollSlot);
    CItem* pItem = G_CDataManager()->find_item((int)item.m_addInfo);  // 副本 +2 = 物品索引
    if (pItem == 0)
        return 0x16;
    if (!pItem->is_stackable())
        return 0x16;
    // +0x32c 属派生类 CStackableItem（CItem 尺寸 0x148，CStackableItem 0x3f0）；
    // is_stackable 通过后必为 CStackableItem，字段 m_int32c（CStackableItem.h:365）。
    if (static_cast<CStackableItem*>(pItem)->m_int32c == -1)
        return 0x16;
    // ORIG 0x085c53d2 `mov -0x42(%ebp),%eax; test %eax,%eax; jg`：直读副本
    // +0x07 字段（m_addInfo2），不调 get_add_info（P167 EXTRA ×1）。
    if (item.m_addInfo2 <= 0)                                  // 副本 +7 = 数量
        return 0x16;
    return 0;
}