bool CUser::removeRentalItemInfoFromInvenBuf(SIG_SAVE_INVENTORY* sig, int param)  // b28c-L5 SIGFIX
{
    if (sig == 0)
        return 0;

    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (getCurrentCharacRentalInfo(rentalList, param) != 1)
        return 0;

    for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it = rentalList.begin();
         it != rentalList.end(); ++it)
    {
        InstanceRentalSystem::RentalInfo& rental = *it;
        bool bInEquip = false;
        int slot = 0;
        // 2026-09-10（G3CUser-B，P183/P0 删发明具名量）：
        //   ORIG 0x08677607 `mov 0x4(%eax),%eax; mov %eax,%ebx`（rental.m_itemIndex
        //   **无槽**、%ebx 跨调用保活）与 0x0867760c `call getCurCharacInvenR; mov %eax,
        //   (%esp); call CheckExistItem...`（inven **无槽**、直传 this）⇒ 我方
        //   `int itemIndex` / `const CInventory* inven` 两个具名量 ORIG 均无；
        //   else 分支 0x08677638 亦从 rental 重算 m_itemIndex、0x0867767d 重调
        //   getCurCharacInvenR ⇒ 全部表达式内联。
        if (getCurCharacInvenR()->CheckExistItemInInventoryOrEquipment(
                rental.m_itemIndex, slot, bInEquip) == 1)
        {
            rental.m_inEquip = bInEquip;
            if (bInEquip == false)
            {
                sig->m_invenType1[slot].reset();   // 背包（类型1）槽
            }
            else
            {
                slot -= 10;
                sig->m_invenType0[slot].reset();   // 装备（类型0）槽
            }
        }
        else
        {
            LogManager::logFormat(
                1, "user.cpp",
                __PRETTY_FUNCTION__,
                0x6d15,
                "Rent error. CUser::removeRentalItemInfoFromInvenBuf return false. Acc(%d), itemIndex(%d)",
                get_acc_id(), rental.m_itemIndex);
            getCurCharacInvenR()->ShowAllExistItemsForErrorCheck();
        }
    }

    setCurrentCharacRentalInfo(rentalList, param);
    return 1;
