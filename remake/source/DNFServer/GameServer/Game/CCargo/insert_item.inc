 // ---- 插入 ----
int CCargo::insert_item(const Inven_Item& item)
{
    int slot = 0;
    // [b28c-16 P0 清绑] 原 register asm("eax") 绑定违反 P0 全域禁则；callee
    // isEquipableItemType 返回 bool ⇒ P168a① 裸形 `if (!call())` 直产
    // `call; xor $0x1,%eax; test %al,%al; je`，无需绑定。
    if (!item.isEquipableItemType()) {
        for (; slot < 0x98; ++slot) {
            if (_AddStackItem(item, slot) >= 0)
                return slot;
        }
    }
    slot = 0;
    // ORIG 尾置条件 for（setle 形，与第一循环同构）。
    for (; slot < 0x98; ++slot) {
        if (m_pItems[slot].m_addInfo == 0)
        {
            _AddItem(item, slot);
            return slot;
        }
    }
    return -1;
}