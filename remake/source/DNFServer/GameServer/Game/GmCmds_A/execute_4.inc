void CAddItemByMail::execute()
{
    CUser* user = GetUser();
    // [b28d-solo-GmCmds_A] ORIG +0x02c 把 find_item 结果存入具名槽 -0x2c
    // （`mov %eax,-0x2c; cmpl $0,-0x2c; je`），内联 test/sete 形不符；
    // 实参用 m_itemIdx(+8) 直读（无 itemId 槽，其槽 -0x28 在两次 strncpy 后写）。
    CItem* found = G_CDataManager()->find_item(m_itemIdx);
    if (found == 0)
    {
        return;
    }

    // [b28d-solo-GmCmds_A] P312：ORIG 对 title[0x100](-0x182)/sender[0x15](-0x45)
    // 发内联对齐剥离清零（无 memset call，p167 EXTRA memset×2）。
    // [b28d-solo-2] P198①：ORIG +0x03e 在数组清零**之前**发 `movl $0,-0x28`
    // （itemId 预清），+0x16f 才写 m_itemIdx ⇒ 两句式（声明=0 + 后赋值）。
    int itemId = 0;
    char title[0x100] = {0};
    char sender[0x15] = {0};
    strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_144", 0),
            0xff);
    strncpy(sender, g_scriptStringManager_.findString(4, "game_server_msg_143", 0),
            0x14);

    // [b28d-solo-GmCmds_A] ORIG 声明/初始化序：itemId(-0x28) → addInfo(-0x24)
    // → Inven_Item C1(-0x82) → count(-0x20)。
    itemId = m_itemIdx;
    int addInfo = 1;
    Inven_Item item;
    int count = 0;
    if (itemId == 0)
    {
        count = addInfo;
    }
    else if (itemId == 1)
    {
        item.m_addInfo = 1;          // +0x02
        item.m_field1 = 2;           // +0x01
        item.m_addInfo2 = addInfo;   // +0x07
        item.m_fieldb = 1;           // +0x0b
        item.ResetItemAttr();
    }
    else
    {
        CItem* proto = G_CDataManager()->find_item(itemId);
        if (proto == 0)
        {
            return;
        }
        item.m_addInfo = itemId;     // +0x02
        proto->make_item(item);      // ORIG vtbl[2]
        item.set_add_info(addInfo);
    }

    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender, item, static_cast<unsigned int>(count),
        static_cast<unsigned int>(user->getCurCharacNo()), title,
        static_cast<int>(strlen(title)), 0u, user->GetServerGroup(), false, false);
