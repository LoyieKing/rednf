void GrowthEquipEvent::GiftGrowthWeaponBox(CUser& user) const
{
    // [实测] ORIG 0x080f70xx `mov %eax,-0x18(%ebp)` + `mov -0x18(%ebp),%eax;
    // movzbl (%eax)` ⇒ data **是具名局部并占槽**（ORIG 的 `push %esi` 属 characNo，
    // 见下）；`register` 形（用 %ebx）实测反劣 ⇒ 保持具名。
    char* data = static_cast<char*>(user.getGrowthWeaponEventdata());
    if (*data == '\x01')            // 已领取过：直接返回
        return;
    *data = '\x01';

    const int giftBoxIdx = G_CDataManager()->m_growthWeapon.m_giftBox;

    Inven_Item item;                // ORIG Inven_Item::Inven_Item() 0x080cb854
    CItem* itemBase = G_CDataManager()->find_item(giftBoxIdx);

    int serverGroup;
    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        serverGroup = G_CEnvironment()->get_server_group();
    else
        serverGroup = (int)user.GetServerGroup();

    if (itemBase == 0)
        return;

    // vtable+0x08 = CItem::make_item(Inven_Item&) const
    itemBase->make_item(item);
    item.set_add_info(1);           // ORIG Inven_Item::set_add_info(item,1)

    // 邮件正文：RDAR 字符串 game_server_msg_278（strncpy 0xff 忠实保留）。
    char title[256];
    strncpy(title,
            g_scriptStringManager_.findString(
                4, "game_server_msg_278", static_cast<bool*>(0)),
            0xff);

    if (giftBoxIdx != 0)
    {
        const int titleLen = static_cast<int>(strlen(title));
        // [实测] ORIG 0x080f7179 `mov %eax,%esi` + 0x080f71cb `mov %esi,0x14(%esp)`
        // （以 %esi 跨 findString call 保活），但加裸 `register` 提示实测指令数
        // 115→117 反劣（4.4 未按预期选 %esi）⇒ 保持具名；本件残差归 N82/N7
        // （帧 0x180 vs 0x190、data 槽 -0x18 vs -0x1c）。
        const unsigned int characNo =
            static_cast<unsigned int>(user.getCurCharacNo());
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(
                4, "game_server_msg_277", static_cast<bool*>(0)),
            item, 0u, characNo, title, titleLen, 0u,
            static_cast<ENUM_SERVER_GROUP>(serverGroup), false, false);
    }

    DB_EventSaveGrowthWeapon::makeRequest(user.get_acc_id(),
                                          static_cast<short>(serverGroup));
    user.SendEventMailAlaramTooltip();
