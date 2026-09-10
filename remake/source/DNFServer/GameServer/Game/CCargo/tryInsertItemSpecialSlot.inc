int CCargo::tryInsertItemSpecialSlot(const Inven_Item& item, int slot) const
{
    if (!IsOperatorable(slot))
        return -2;
    if (!item.isEquipableItemType()) {
        // [b28d-solo-CCargo] ORIG 0x0850b570 块序：循环分支（ret2@-0x10, i@-0x14）
        // 物理在前、slot 分支（ret1@-0xc）在后（`cmp; je +0xac`）⇒ 源码写
        // `if (addInfo != item.addInfo) { 循环 } else { slot }`（4.4 分支块序与
        // 源码 if/else 同向，取反条件即翻正；语义等价）。
        if (m_pItems[slot].m_addInfo != item.m_addInfo) {
            for (int i = 0; i < 0x98; ++i) {
                int ret = _tryAddStackItem(item, i);
                if (ret >= 0)
                    return i;
                if (ret == -10)
                    return -1;
            }
        } else {
            int ret = _tryAddStackItem(item, slot);
            if (ret >= 0)
                return slot;
            if (ret == -10)
                return -1;
        }
    }
    // [b28d-solo-CCargo] ORIG 尾段 `test %eax,%eax; jne <ret -1>` + slot 返回块
    // 在前 jmp、-1 块在最后直落 ⇒ 源码写 `if (==0) return slot; return -1;`。
    if (m_pItems[slot].m_addInfo == 0)
        return slot;
    return -1;
