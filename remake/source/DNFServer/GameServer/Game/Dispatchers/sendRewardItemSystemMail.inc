// ==== Dispatcher_RequestIngameAdvertisement::sendRewardItemSystemMail (ORIG 0x081e4b6c) ====
// 头声明缺失：宿主需在 Dispatchers.h 补
//   void sendRewardItemSystemMail(CUser* user, unsigned long itemIdx, int count);
void Dispatcher_RequestIngameAdvertisement::sendRewardItemSystemMail(
    CUser* user, unsigned long itemIdx, int count)
{
    // [b28c-34] ORIG 0x081e4cc4/0x081e4cc5 两处 je 目标相邻（共享尾岛 +0x159）；
    // ORIG 岛数 = 1（body 尾 `jmp +0x159; nop`，两 je 直达）⇒ 单早返 + 正向包裹。
    if (user == 0)
        return;
    CItem* find = G_CDataManager()->find_item(static_cast<int>(itemIdx));   // -0xc（CItem* 指针槽）
    if (find != 0)
    {
        Inven_Item item;                                        // ORIG C1 W 0x080cb854
        item.m_addInfo = static_cast<int>(itemIdx);             // +0x02（item idx）
        G_CDataManager()->m_itemList->create_item(ENUM_ITEM_CREATE_TYPE_0, item, count);

        // RDAR 字符串：发件人名（≤0x1d）/ 正文（≤0xff），strncpy 不手动补 NUL（同 ORIG）
        char sender[0x1e];                                      // ORIG local [30] @ -0x2a
        strncpy(sender,
                g_scriptStringManager_.findString(4, "game_server_msg_295", 0),
                0x1d);
        char body[0x100];
        strncpy(body,
                g_scriptStringManager_.findString(4, "game_server_msg_296", 0),
                0xff);

        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            sender, item, 0u,
            static_cast<unsigned int>(user->getCurCharacNo()),
            body, static_cast<int>(strlen(body)), 0u,
            user->GetServerGroup(), false, false);
    }
}