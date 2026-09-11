// ORIG T 0x08676bec：次元租借物品 → SIG_SAVE_DIMENSION_RENT_ITEM（最多 12 个 8 字节条目）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08676bec | _ZN5CUser20GetDimensionRentInfoEP28SIG_SAVE_DIMENSION_RENT_ITEM
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
        // 2026-09-11 CuA A组 P183/P0：ORIG 0x08676cd4 调用点无 inven 指针槽
        //   （`call getCurCharacInvenR` 返回值直接 `mov %eax,(%esp)` 作 this，
        //   我方旧具名 `inven` 占 -0xc 槽 ⇒ 全帧槽号整体 +4、多 1 条 load）
        //   ⇒ 链式内联（同 GetPcRoomRentInfo 已落形）。
        if (getCurCharacInvenR()->CheckExistItemInInventoryOrEquipment(
                rental.m_itemIndex, slot, bInEquip) == 1)
        {
            sig->m_entry[index].m_itemIndex = rental.m_itemIndex;
            sig->m_entry[index].m_field4 = rental.m_field8;
            sig->m_entry[index].m_field5 = rental.m_field9;   // ORIG movzbl 0x9
            sig->m_entry[index].m_inEquip = bInEquip;
            sig->m_entry[index].m_field7 = rental.m_fieldB;   // ORIG movzbl 0xb
            ++index;
            if (index > 11)
                return 0;
        }
    }
    return 1;
}