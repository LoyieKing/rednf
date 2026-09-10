CItem* CUser::_GetRecipeItem(unsigned long itemIdx, bool byIndex, int& outSlotRef)
{
    CItem* ret;
    if (byIndex)
    {
        ret = G_CDataManager()->find_item((int)itemIdx);
        if (ret == 0)
            return 0;
    }
    else
    {
        // [b28d-solo-CUserOnlyOrig P278 试形] ORIG 字段访问直接 `mov -0x4b(%ebp),%eax`
        // （sret 槽 -0x4d + disp，无引用指针槽）⇒ 值类型局部直构进 sret 槽；
        // `const Inven_Item&` 反产 -0xc 指针槽 + `mov -0xc,%eax; mov 0x2(%eax)` 两段。
        const Inven_Item slot = const_cast<CInventory*>(getCurCharacInvenR())
                                    ->GetInvenSlot(1, (int)itemIdx);
        if (slot.m_addInfo == 0)                            // ORIG mov -0x4b(+0x02 dword) test
            return 0;
        outSlotRef = slot.get_add_info();
        ret = G_CDataManager()->find_item(slot.m_addInfo);
        if (ret == 0)
            return 0;
    }
    if (!ret->is_stackable())
        return 0;
    // [b28d-solo-CUserOnlyOrig P0] ORIG 有具名 `CStackableItem* stack` 槽（-0xc，
    // `mov -0x10,%eax; mov %eax,-0xc; mov -0xc,%eax; mov 0x2b0(%eax)`），返回也走该槽。
    CStackableItem* stack = (CStackableItem*)ret;
    if (stack->m_nItemType != 3)                    // ORIG 读 +0x2b0（非 GetMonsterCardId +0x2b8）
        return 0;
    return stack;
