int CInventory::CheckEquipmentItemCondition(INVEN_TYPE type, short slot,
                                            stEquipConditionForChronicle& cond) const
{
    // +0x00: GetInvenSlot(type, slot) —— 返回 Inven_Item 值拷贝。
    Inven_Item item = GetInvenSlot(static_cast<int>(type), slot);

    // GetItemType(item idx) != 1(EQUIP) → return 0x11
    int itemType = GetItemType(item.m_addInfo);
    if (itemType != 1)
        return 0x11;

    // find_item(idx)：ORIG 无类型转换直接调 CEquipItem::get_endurance，
    // 用 static_cast<CEquipItem*> 保持 ABI 等价。
    CEquipItem* equip =
        static_cast<CEquipItem*>(G_CDataManager()->find_item(item.m_addInfo));
    if (!equip)
        return 0x11;

    // ORIG 顺序：先取 abilityType 存栈(-0xd)，再 isIdentified 判定。
    // b28c-L2: 局部与目标字段同型化（uchar=uchar）翻转 store 装载序为 ORIG 的
    // LHS 地址先形（0x850851a `mov 0x14(%ebp),%eax` 先于 `movzbl -0xd,%edx`，P211）。
    unsigned char abilityType = item.m_amp.getAbilityType();
    if (!item.m_amp.isIdentified())
        return 0x13;

    // item+0x0b（endurance ushort）== equip->get_endurance() 才通过。
    int endurance = equip->get_endurance();
    if (static_cast<int>(item.m_endurance) != endurance)
        return 0xd8;

    // ---- 输出块（对应 asm 逐偏移写）----
    cond.m_upgrade = item.GetUpgrade();                     // cond[0] ← Inven_Item::GetUpgrade
    if (abilityType == 0)
    {
        cond.m_hasAmplify = 0;                              // cond[1]
        cond.m_amplifyType = 0;                             // cond[2]
        cond.m_amplifyValue = 0;                            // *(ushort*)&cond[4]
    }
    else
    {
        cond.m_hasAmplify = 1;                              // cond[1]
        cond.m_amplifyType = abilityType;                   // cond[2]（原字节）
        cond.m_amplifyValue = item.m_amp.getAbilityValue(); // *(ushort*)&cond[4]
    }

    // cond[6]：Inven_Item 字节0（m_field0）非零 → 1，否则 0。
    if (item.m_seal)
        cond.m_hasField0Item = 1;
    else
        cond.m_hasField0Item = 0;

    // cond[7] ← Inven_Item::GetReSealCount()
    cond.m_reSealCount = item.GetReSealCount();

    // cond+8(int) ← Inven_Item+0x0d 联合整体读（m_fieldd）。
    cond.m_extendInfo = item.m_extendInfo;

    return 0;
}