bool CUser::removeRentalItemInfoFromInvenLoadBuf(SIG_LOAD_INVENTORY* sig, int param)  // b28c-L5 SIGFIX
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
        // b28d-G3C 2026-09-10（P59/P222 分支极性 + P183 去具名指针）：
        //   ORIG 0x08677819 `xor $0x1,%eax; test %al,%al; je +0x10e(=OK块)`
        //   —— bool 取反形（我方原写 `== 1` 只产 `test %al; je`，缺 xor）；
        //   ORIG 0x086777fa 调 getCurCharacInvenR 后**不落槽**直接
        //   `mov %eax,(%esp)`（我方原有具名 `const CInventory* inven` 独占
        //   -0xc 槽：多 1 store + 1 load，并把 rental 引用挤到 -0x10）。
        if (!getCurCharacInvenR()->CheckExistItemInInventoryOrEquipment(
                rental.m_itemIndex, slot, bInEquip))
        {
            LogManager::logFormat(
                1, "user.cpp",
                __PRETTY_FUNCTION__,
                0x6d54,
                "Rent error. CUser::removeRentalItemInfoFromInvenLoadBuf return false. Acc(%d), itemIndex(%d)",
                get_acc_id(), rental.m_itemIndex);
            getCurCharacInvenR()->ShowAllExistItemsForErrorCheck();
        }
        else
        {
            rental.m_inEquip = bInEquip;
            if (bInEquip)
                sig->m_equipMirror[slot].reset();
            else
                sig->m_invenType1[slot].reset();
        }
    }
    setCurrentCharacRentalInfo(rentalList, param);
    return 1;
