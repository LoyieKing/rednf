// ORIG CheckUnusableEquipment 0x8562bdc：
//   空 item→0；item->m_addInfo==0→1；find_item 失败→0；
//   可堆叠→0；equipment type==0→1；
//   CheckUnusableEquipmentInfo(type, usableLevel)==1 时再以
//   vtable+0xc（GetItemType）与 get_index 查 CheckUnusableItem 判定；
//   其余返回 0。
// [b28d-solo-CFairMatch] N7 | 参数槽已修（static，P245）；残余两处纯寄存器角色：
// ①CheckUnusableEquipmentInfo 的 arg3→%esi / arg2→%ebx（ORIG）vs arg3→%ebx /
// arg2→%esi（4.4）；②间接虚调目标 ORIG 用 %edx vs 4.4 用 %eax（0x08562cc8 段）。
// 本机双后端实测均不可治：4.4.7 与 4.4.6（-B cc1plus446bin，含 4.4.6 头集）读数
// 完全相同（identical=37 ae=24 near=1 diff=3）。虚槽 +0xc 已按 P193/P282 核对
// _ZTV10CEquipItem vptr+0xc = CEquipItem::GetItemType 0x08514d26（语义正确）。
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
}