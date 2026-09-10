int CUser::GetDimensionRentInfo(SIG_SAVE_DIMENSION_RENT_ITEM* sig)
{
    if (sig == 0)
        return 0;
    sig->clear();
    if (!m_rentalFlag1)   // ORIG 0x8676a10/0x8676c12 movzbl; xor $0x1,%eax; test;je（bool 取反形）
        return 1;

    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (getCurrentCharacRentalInfo(rentalList, 2) != 1)
        return 1;

    int index = 0;
    // 2026-09-10（FL2CUser cv 轴，同 GetPcRoomRentInfo）：ORIG 0x08676bec p167 实形
    //   为显式 `const_iterator it = rentalList.begin()`（MISS `C2IPS2_E` 转换构造；
    //   EXTRA 侧非 const de/pp/ne 三条）⇒ 改 const_iterator + const 引用。
    for (std::vector<InstanceRentalSystem::RentalInfo>::const_iterator it =
             rentalList.begin();
         it != rentalList.end(); ++it)
    {
        const InstanceRentalSystem::RentalInfo& rental = *it;
        bool bInEquip = false;
        int slot = 0;
        const CInventory* inven = getCurCharacInvenR();
        if (inven->CheckExistItemInInventoryOrEquipment(rental.m_itemIndex, slot, bInEquip) == 1)
        {
            sig->m_entry[index].m_itemIndex = rental.m_itemIndex;
            sig->m_entry[index].m_field4 = rental.m_field8;
            sig->m_entry[index].m_field5 = rental.m_field9;   // ORIG movzbl 0x9
            sig->m_entry[index].m_inEquip = bInEquip ? 1 : 0;
            sig->m_entry[index].m_field7 = rental.m_fieldB;   // ORIG movzbl 0xb
            ++index;
            if (index > 11)
                return 0;
        }
    }
    return 1;
