int CUser::process_use_unlocked_multibox(int keySlot, int boxSlot,
                                         std::vector<std::pair<Inven_Item, bool> >& dest)
{
    Inven_Item key;
    key = getCurCharacInvenR()->GetInvenSlot(1, keySlot);
    register int addInfo = key.m_addInfo;   // W8A 范式：先物化到 callee-saved
    // ORIG 0x08675dd0-0x08675de6：单条 `mov -0x57,%eax; mov %eax,%ebx; call G_CDataManager;
    // mov %ebx,0x4(%esp); mov %eax,(%esp); call find_item` —— 仅 1 次 G_CDataManager 调用
    CItem* boxDef = G_CDataManager()->find_item(addInfo);   // -0x10
    int err = check_unlocked_multibox_error(boxDef, boxSlot);
    if (err != 0) return err;

    std::vector<std::pair<Inven_Item, bool> > lottery;             // -0x1c
    lottery.clear();
    ((CStackableItem*)boxDef)->get_multibox_lottery_item(&lottery);

    err = check_insert_lottery_reward(boxDef, lottery, dest);
    if (err != 0) return err;

    getCurCharacInvenW()->delete_item((INVEN_TYPE)1, keySlot, 1,
                                      (eItemDelReason)3, true);
    return 0;
