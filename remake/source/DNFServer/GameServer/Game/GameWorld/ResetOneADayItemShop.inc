void GameWorld::ResetOneADayItemShop()
{
    // b28c-lane3：P223 空块岛复现。ORIG 0x86cf82c `jne 86cf901`（尾 nop 岛）、
    // 0x86cf886 `jne 86cf8ac`（内层 nop 岛）、0x86cf8aa `jmp`、0x86cf8ff `jmp`
    // 共 4 处；4.4 对 `if(c){}else{}` 恒 jump-thread 掉 jmp+nop（-2 条）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x6d)->IsEventing(0) ^ 1)
        goto L_outer;
    G_CDataManager()->m_itemShop->updateOneADayItemList();
    // ORIG 0x86cf8c2 ppEi 后缀 ++（temp 槽 -0x10）；it=-0x18 / end=-0x14（技巧25）
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() <= 2)
            goto L_inner;
        user->SendOneADayItemShopIndex(
            G_CDataManager()->m_itemShop->GetOneADayItemList());
    L_inner: ;
    }
L_outer: ;
