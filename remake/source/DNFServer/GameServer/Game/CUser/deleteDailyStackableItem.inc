void CUser::deleteDailyStackableItem(std::vector<std::pair<int, int> >& list,
                                     int a, int b)
{
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
