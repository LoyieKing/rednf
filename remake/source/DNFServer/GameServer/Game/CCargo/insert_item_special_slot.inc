int CCargo::insert_item_special_slot(Inven_Item& item, int slot)
{
    if (!IsOperatorable(slot))
        return -2;
    // [b28c-16 P0 清绑] 同 insert_item：bool callee 裸形（P168a①）。
    if (!item.isEquipableItemType()) {
        // ORIG 块排布：cmp je 正跳到 then 体（物理在后）；声明序决定槽
        // ret1(-0xc) → ret2(-0x10) → i(-0x14)。
        // 4.4 同层变量按声明逆序分配槽（先声明 → 深地址），故逆序声明。
        int i;
        int ret2;
        int ret1;
        if (m_pItems[slot].m_addInfo != item.m_addInfo) {
            for (i = 0; i < 0x98; ++i) {
                ret2 = _tryAddStackItem(item, i);
                if (ret2 >= 0)
                    return i;
                if (ret2 == -10)
                    return -1;
            }
        } else {
            ret1 = _AddStackItem(item, slot);
            if (ret1 >= 0)
                return slot;
            if (ret1 == -10)
                return -1;
        }
    }
    if (m_pItems[slot].m_addInfo == 0) {
        _AddItem(item, slot);
    } else {
        return -1;
    }
    return slot;
