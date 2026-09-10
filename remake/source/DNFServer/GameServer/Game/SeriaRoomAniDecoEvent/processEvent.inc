int SeriaRoomAniDecoEvent::processEvent(CUser& user)    // 0x0827ba52
{
    seriaRoom_AniDeco deco = user.getSeriaRoomAniDecoInfo();
    int itemIdx =
        G_CDataManager()->getSeriaRoomAniDecoRewardItem(deco.m_field2);
    if (itemIdx == 0)
        return 0;

    Inven_Item item;
    CItem* src = G_CDataManager()->find_item(itemIdx);
    if (src != 0)
    {
        // ORIG 0x0827bad5 `mov %eax,-0x5f`：item(@-0x61)+0x02 ⇒ 原码直接写
        // Inven_Item::m_addInfo 字段（非独立 int 局部；make_item 不写 +0x02，
        // CEquipItem/CStackableItem::make_item 写点已核）。
        item.m_addInfo = src->get_index();
        src->make_item(item);
        item.set_add_info(1);

        char title[256];
        strncpy(title,
                g_scriptStringManager_.findString(4, "game_server_msg_291", 0),
                0xff);
        if (item.m_addInfo != 0)
        {
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, "game_server_msg_289", 0),
                item, 0, (unsigned int)user.getCurCharacNo(),
                title, (int)strlen(title), 0,
                static_cast<ENUM_SERVER_GROUP>(user.GetServerGroupToPvP()),
                false, false);
        }
    }
    return 0;
}
