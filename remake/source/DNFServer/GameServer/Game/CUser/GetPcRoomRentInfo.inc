int CUser::GetPcRoomRentInfo(SIG_SAVE_PC_ROOM_RENT_ITEM* sig)
{
    if (sig == 0)
        return 0;
    sig->clear();
    if (!m_rentalFlag1)   // ORIG 0x8676a10/0x8676c12 movzbl; xor $0x1,%eax; test;je（bool 取反形）
        return 1;

    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (getCurrentCharacRentalInfo(rentalList, 1) != 1)
        return 1;

    int index = 0;
    // 2026-09-10（FL2CUser cv 轴）：ORIG 0x086769ea p167 实形 = 迭代器**混合比较**
    //   `_ZN9__gnu_cxxneIPKN...RentalInfoEPS2_St6vectorIS2_SaIS2_EEEEbRKNS_17__normal_iteratorIT_T1_EERKNS9_IT0_SB_EE`
    //   （第一实参 const_iterator、第二实参 iterator = rentalList.end()）⇒ 原码
    //   **显式 `const_iterator it = rentalList.begin()`**（MISS `C2IPS2_E` 即
    //   iterator→const_iterator 的转换构造；EXTRA 侧非 const 的 de/pp/ne 三条消失）。
    for (std::vector<InstanceRentalSystem::RentalInfo>::const_iterator it =
             rentalList.begin();
         it != rentalList.end(); ++it)
    {
        const InstanceRentalSystem::RentalInfo& rental = *it;
        bool bInEquip = false;
        int slot = 0;
        // 2026-09-10 gA（A 组）：ORIG 0x08676ab2..0x08676ad2 无 inven 指针槽
        //   （`call getCurCharacInvenR` 返回值直接 `mov %eax,(%esp)` 作 this；实参先
        //   物化：`lea -0x29(%ebp),%edx; mov %edx,0xc(%esp)`（&bInEquip）、
        //   `lea -0x30(%ebp)`（&slot）、`mov 0x4(%eax),%ebx`（rental.m_itemIndex））
        //   ⇒ 删具名 inven，改链式内联（P183 地址重物化铁律）。
        if (getCurCharacInvenR()->CheckExistItemInInventoryOrEquipment(
                rental.m_itemIndex, slot, bInEquip) == 1)
        {
            sig->m_entry[index].m_itemIndex = rental.m_itemIndex;
            sig->m_entry[index].m_field4 = rental.m_field8;
            sig->m_entry[index].m_field5 = rental.m_field9;   // ORIG movzbl 0x9
            sig->m_entry[index].m_inEquip = bInEquip ? 1 : 0;
            sig->m_entry[index].m_field8 = rental.m_rentItemId;  // ORIG mov 0xc
            ++index;
            if (index > 1)
                return 0;
        }
    }
    return 1;
