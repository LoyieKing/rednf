int CUser::process_use_locked_multibox(int boxSlot, int keySlot,
                                       std::vector<std::pair<Inven_Item, bool> >& dest)
{
    Inven_Item key;
    // [b28d-22 修法+真语义 bug] ORIG 0x08675aed `mov 0xc(%ebp),%ecx; mov %ecx,0xc(%esp)`
    // ⇒ GetInvenSlot 第二实参 = boxSlot（旧版传 keySlot 读错槽）。
    key = getCurCharacInvenR()->GetInvenSlot(1, boxSlot);

    // ORIG 0x08675b8a-0x08675b9b 仅一次 G_CDataManager 调用（旧版多一条独立调用）。
    CItem* boxDef = G_CDataManager()->find_item(key.m_addInfo);   // -0x18

    CBoosterGage* gage = (CBoosterGage*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)(0x11));
    bool overGage = gage->check_max_booster_gage(key.m_addInfo);  // -0x11

    int err = check_locked_multibox_error(boxDef, keySlot, overGage);
    if (err != 0) return err;

    std::vector<std::pair<Inven_Item, bool> > lottery;             // -0x24
    lottery.clear();
    ((CStackableItem*)boxDef)->get_multibox_lottery_item(&lottery);

    err = check_insert_lottery_reward(boxDef, lottery, dest);
    if (err != 0) return err;

    int keyCount = static_cast<CStackableItem*>(boxDef)->m_multiBox.m_fieldc;  // +0x1f0 = m_multiBox(+0x1e4).m_fieldc(+0x0c)
    getCurCharacInvenW()->delete_item((INVEN_TYPE)1, boxSlot, 1,
                                      (eItemDelReason)3, true);
    getCurCharacInvenW()->delete_item((INVEN_TYPE)1, keySlot, keyCount,
                                      (eItemDelReason)3, true);
    return 0;
