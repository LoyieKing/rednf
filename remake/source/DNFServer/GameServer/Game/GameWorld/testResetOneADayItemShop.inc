void GameWorld::testResetOneADayItemShop()
{
    // b28c-lane3：P223 空块岛复现（同 ResetOneADayItemShop，ORIG 0x086cf904）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x6d)->IsEventing(0) ^ 1)
        goto L_outer;
    G_CDataManager()->m_itemShop->testupdateOneADayItemList();
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
}