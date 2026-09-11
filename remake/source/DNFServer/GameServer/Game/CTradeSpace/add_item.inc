// ---- add_item @ 0x08529aa2 ----
int CTradeSpace::add_item(int traderIdx, TradeSlot slot, int count)
{
    // b28d-solo-CTradeSpace：按 ORIG 0x08529aa2 还原。
    // [K2 2026-09-11 复验] fresh .o：gap 仍 +2（our 191 / orig 189）；2 处 =
    //   ORIG 地址链尾 `add 0x8(%ebp),%eax`（dst 直落 caller-saved %eax，cmp/
    //   memcpy 共用）vs 我方 `mov %eax,%ebx; add 0x8(%ebp),%ebx`（register 指针
    //   落 callee-saved = N7/P231 跨版本寄存器分配）。交换 cmp 操作数与删
    //   register（栈槽 +2/处）已实测不翻 ⇒ N7 封顶；p167 14/14 仅 C1/C2 别名差。
    //   ①dst 用纯 register 指针（P231，非绑定、P0 合规）复现 ORIG 单次全链
    //     计算 + cmp/memcpy 共用的无槽形（余 26 ndiff 为 N7：ORIG dst 落 %eax/
    //     &slot 落 %edx，4.4 分别落 %ebx/%eax）；②两个 trace 用全表达式临时链
    //     （P227，ORIG m_addInfo 先入 %ebx 跨 ctor 保活）；③newCount 声明/赋值
    //     分两句复现 ORIG「先存槽后 add mem」序（P198①）。
    int idx;
    if (count < 0)
        return -1;
    idx = -1;
    lock();
    if (slot.m_item.isEquipableItemType()) {
        idx = get_empty_itemslot(traderIdx);
        if (idx != -1) {
            register TradeSlot* dst = &m_slotsData[traderIdx][idx];
            // N7：4.4 对 register 指针局部恒落 callee-saved %ebx ⇒ 地址链尾 +1 条
            //   `mov %eax,%ebx`（ORIG 4.1 落 caller-saved %eax 直落）；两处各 +1
            //   = gap +2。交换 cmp 操作数 / 删 register（退化栈槽 +2/处）均实测不翻。
            if (dst != &slot)
                memcpy(dst, &slot, 0x46);
        }
    } else {
        idx = check_item_exist(traderIdx, slot.m_item.m_addInfo);
        if (idx != -1) {
            int newCount =
                m_slotsData[traderIdx][idx].m_item.m_addInfo2;
            newCount += count;
            m_slotsData[traderIdx][idx].m_item.set_add_info(newCount);
            if (newCount > 999) {
                cMyTrace(__PRETTY_FUNCTION__, 0xd1, 5)(
                    "CTradeSpace::add_item, ITEM #%d, %d",
                    m_slotsData[traderIdx][idx].m_item.m_addInfo, newCount);
            }
        } else {
            idx = get_empty_itemslot(traderIdx);
            if (idx != -1) {
                register TradeSlot* dst = &m_slotsData[traderIdx][idx];
                if (dst != &slot)
                    memcpy(dst, &slot, 0x46);
                m_slotsData[traderIdx][idx].m_item.set_add_info(count);
                if (count > 999) {
                    cMyTrace(__PRETTY_FUNCTION__, 0xde, 5)(
                        "CTradeSpace::add_item, ITEM #%d, %d",
                        m_slotsData[traderIdx][idx].m_item.m_addInfo, count);
                }
            }
        }
    }
    unlock();
    return idx;
}