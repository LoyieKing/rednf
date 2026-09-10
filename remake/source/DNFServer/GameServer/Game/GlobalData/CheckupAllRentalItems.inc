int RentalMachinePcRoom::CheckupAllRentalItems(
    CUser& user, const InstanceRentalSystem::RentInput& input)
{
    int inventoryNeed = 0;
    // ORIG 0x08270d30：与 Dimension::CheckupAllRentalItems 同构 —— begin() 隐藏
    // 返回指针迭代器 @-0x1c、循环条件后置（end()+operator!=）、体内 operator*。
    for (std::vector<InstanceRentalSystem::RentInput::RentInElem>::const_iterator
             it = input.m_elems.begin();
         it != input.m_elems.end(); ++it)
    {
        const InstanceRentalSystem::RentInElem& elem = *it;
        EventRewardItemInfo info;
        if (!pc_room::GetRentItemInfo(user, (char)elem.m_sectionIdx,
                                      (char)elem.m_itemIdx, info, 1))
        {
            return 1;
        }
        CItem* script = G_CDataManager()->find_item(info.m_field0);
        if (script == 0)
        {
            return 1;
        }

        Inven_Item probe;
        probe.m_addInfo = info.m_field0;
        script->make_item(probe);                    // ORIG 虚表槽 +0x08

        if (elem.m_bEquip != 0)
        {
            if (user.getCurCharacInvenR()->tryInsertItemIntoEquipment(probe) < 0)
            {
                return 0xcb;
            }
        }
        else
        {
            // ORIG 0x08270f2a/0x08270f4d：`shr $0x1f,%eax; test %al,%al; je <++>`
            // —— 负值走 return，非负 fall 到 `addl $0x1` ⇒ 源码写 `< 0` 早退，
            // 不写 `>= 0 {…} else {…}`（后者 4.4 会多出 `not %eax` 反相）。
            if (user.getCurCharacInvenR()->tryInsertItemIntoInventory(probe) < 0)
            {
                return 0xcb;
            }
            ++inventoryNeed;
        }
    }
    // 同 Dimension 版：ORIG 尾 if-return 形（P47/P191）。
    if (!pc_room::CheckInventoryEmptyCount(user, inventoryNeed))
    {
        return 0xcb;
    }
    return 0;
