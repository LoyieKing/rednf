int CFairMatch::CheckUnusableEquipment(const Inven_Item* item)
{
    if (item == 0)
        return 0;

    // ORIG *(int*)(param_1 + 2)：+0x02 m_addInfo（item idx）
    // P0/P48：ORIG 无 itemIdx 槽往返——条件与 find_item 实参各独立读一次
    //（0x08562bxx 两处 `mov 0x8(%ebp),%eax; mov 0x2(%eax),%eax`）。
    if (item->m_addInfo == 0)
        return 1;

    // P48 no-CSE：ORIG 每处 use 各自 fresh G_CDataManager()（find_item /
    // CheckUnusableEquipmentInfo / CheckUnusableItem 共 3 次）；equipType 也
    // 不在比较与实参间缓存（GetUsableEquipmentType 共 2 次）。
    CItem* found = G_CDataManager()->find_item(item->m_addInfo);
    if (found == 0)
        return 0;

    // P59/P222 正极性（ORIG 0x8562c1x `call is_stackable; test %al; je` 直落
    // 主流程；负极性多 xor $1）；P0：无 equip 具名局部，found 表达式直用。
    if (found->is_stackable())
        return 0;

    if (static_cast<CEquipItem*>(found)->GetUsableEquipmentType() == 0)
        return 1;

    // P168a 裸形（ORIG 0x08562c9d/0x08562ce4 `xor $1; test %al; je`）：
    // bool 返回 callee 用 `if (!call()) return 0;`。
    if (!G_CDataManager()->m_serverParameter.CheckUnusableEquipmentInfo(
            (ENUM_UNUSABLE_EQUIPMENT_TYPE)static_cast<CEquipItem*>(found)->GetUsableEquipmentType(),
            found->getUsableLevel()))
        return 0;
    // ORIG vtable 槽 +0xc：CItem::GetItemType()
    if (!G_CDataManager()->m_serverParameter.CheckUnusableItem(
            (ENUM_EQUIPMENTTYPE)const_cast<const CItem*>(found)->GetItemType(),
            (unsigned long)found->get_index()))
        return 0;
    return 1;
