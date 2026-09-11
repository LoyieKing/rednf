bool COnlinePreliminary::CheckUnusableEquipment(const Inven_Item* item)
{
    if (item == 0)
        return false;
    if (item->m_addInfo == 0)
        return true;

    // P48 no-CSE：ORIG 每处 use 各自 fresh G_CDataManager()（find_item /
    // CheckUnusableEquipmentInfo / CheckUnusableItem 共 3 次）；equipType 也
    // 不在比较与实参间缓存（GetUsableEquipmentType 共 2 次）。
    CItem* found = G_CDataManager()->find_item(item->m_addInfo);
    if (found == 0)
        return false;
    if (found->is_stackable())
        return false;

    /* [b28d-solo-OnlinePreliminaryMatch] ORIG 全函数只有一个 CItem* 槽 -0xc
     * （equip 每次从同一槽重读，无独立 -0x10 槽）⇒ 原码无 equip 具名局部，
     * 删除后两处调用均按 ORIG 从 -0xc 重读（帧 0x20/槽号对齐）。 */
    if (static_cast<CEquipItem*>(found)->GetUsableEquipmentType() == 0)  // W 0x832e036
        return true;
    if (!G_CDataManager()->m_serverParameter.CheckUnusableEquipmentInfo(
            /* [b28d-solo-OnlinePreliminaryMatch] ORIG 寄存器角色：先求值
             * getUsableLevel→%esi、后求值 GetUsableEquipmentType→%ebx；
             * 4.4 裸形恒反 ⇒ 第二实参包语句表达式（P226 实参向）。 */
            (ENUM_UNUSABLE_EQUIPMENT_TYPE)static_cast<CEquipItem*>(found)->GetUsableEquipmentType(),
            ({ found->getUsableLevel(); })))
        return false;
    /* ORIG 经虚表槽 +0xc（GetItemType）取类型后查 CheckUnusableItem；
     * [b28d-solo-OnlinePreliminaryMatch] ORIG @08560a23 尾为 `call; xor $1,%eax;
     * test %al,%al; je <mov $1 岛>; mov $0,%eax; jmp` ⇒ 源形是
     * `if(!call) return false; return true;`（直接 `return call;` 无物化）。 */
    if (!G_CDataManager()->m_serverParameter.CheckUnusableItem(
                 (ENUM_EQUIPMENTTYPE)found->GetItemType(),
                                                  (unsigned long)found->get_index()))
        return false;
    return true;
}