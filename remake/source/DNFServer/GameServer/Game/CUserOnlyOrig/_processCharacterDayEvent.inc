void CUser::_processCharacterDayEvent(unsigned int oldFatigue, unsigned int addFatigue)
{
    unsigned int total = oldFatigue + addFatigue;
    if (!(oldFatigue != 0 && total != 0 && oldFatigue < 0x9du))
        return;

    static const float DIV_DAY = 30.0f;  // ORIG .rodata DAY 常量（0x08cf3960 处值 30.0，已验证）
    int dayA = (int)(oldFatigue / DIV_DAY);
    int dayB = (int)(total / DIV_DAY);
    if (dayA == dayB)
    {
        static const float DIV_WEEK = 156.0f;  // ORIG .rodata WEEK 常量（0x08cf3964 处值 156.0，已验证）
        int weekA = (int)(oldFatigue / DIV_WEEK);
        int weekB = (int)(total / DIV_WEEK);
        if (weekA == weekB)
            return;
    }
    int itemId; int addCount;
    char body[256];
    if (dayA == dayB)                       // 同日跨周
    {
        itemId = 0x1d5e; addCount = 1;
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_75", 0), 0xff);
    }
    else if (dayA == 0 && dayB == 1)
    {
        itemId = 0x1d5c; addCount = 2;
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_71", 0), 0xff);
    }
    else if (dayA == 1 && dayB == 2)
    {
        itemId = 0x1d5c; addCount = 4;
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_72", 0), 0xff);
    }
    else if (dayA == 2 && dayB == 3)
    {
        itemId = 0x1d5c; addCount = 6;
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_73", 0), 0xff);
    }
    else if (dayA == 3 && dayB == 4)
    {
        itemId = 0x1d5d; addCount = 1;
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_74", 0), 0xff);
        CUserEventUtil::updateCharacterDayEvent(this, 1);
    }
    else return;

    char title[21];
    strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_70", 0), 0x14);
    Inven_Item gift;
    // [P0 去缓存] ORIG 086597de `mov -0x24(%ebp),%ebx; call G_CDataManager;
    // mov %ebx,0x4(%esp); mov %eax,(%esp); call find_item` —— 无 dm 具名指针槽
    // ⇒ 旧稿 -0x18 槽使 body/gift/title 各深 4B（-0x17e/-0x7e/-0x41 vs ORIG -0x17a/-0x7a/-0x3d）。
    CItem* proto = G_CDataManager()->find_item(itemId);
    if (proto == 0) return;
    gift.m_addInfo = itemId;
    proto->make_item(gift);                 // virtual vtable+0x08 槽
    gift.set_add_info(addCount);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, gift, 0, getCurCharacNo(), body, strlen(body), 0, GetServerGroup(), 0, 0);
