void CUser::deleteDailyStackableItem(std::vector<std::pair<int, int> >& list,
                                     int a, int b)
{
    // 2026-09-10（G3CUser-B 复验）：槽集 ORIG/我方 各 10 槽、零平移零多余
    //   （100:105 insn）；ORIG 0x0867e7bb `je`（ci==0 跳过）与 0x0867e7d1
    //   `xor $1; jne`（!is_stackable 跳过）确认我方 `ci == 0 || !ci->is_stackable()`
    //   两条独立守卫**有 ORIG 直证**，不可删 ⇒ 残余属跨版本 codegen，判 N。
    if (getCurCharacR() == 0)
        return;
    CInventory* inven = (CInventory*)getCurCharacInvenR();
    for (int slot = a; slot <= b; ++slot)
    {
        Inven_Item item = inven->GetInvenSlot((INVEN_TYPE)1, slot);
        if (item.m_addInfo == 0)
            continue;
        CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
        if (ci == 0 || !ci->is_stackable())
            continue;
        // 2026-09-10（§4.3 去裸偏移）：+0x3c5 日删标记改用具名字段访问
        //   （CStackableItem.h:543 `char m_char3c5`）。ORIG 该点 `movzbl 0x3c5(%eax),%eax`
        //   单条 disp 折叠；原裸偏移形 4.4 恒展成 `add $0x3c5; movzbl (%eax)` 两条。
        if (((CStackableItem*)ci)->m_char3c5 <= 0)
            continue;
        enableSaveInven();
        inven->delete_item((INVEN_TYPE)1, slot, item.m_addInfo2,
                           (eItemDelReason)0x13, true);
        // b28d-solo-CUser p167：ORIG 0x0867dcxx 实例化 `make_pair<ENUM_ITEMSPACE, int&>`
        // （我方 (int) cast 致 `make_pair<int, int&>`）⇒ 去掉 cast（P61/P315）。
        list.push_back(std::make_pair(ENUM_ITEMSPACE_0, slot));
    }
