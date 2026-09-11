// sendOneMatchLineReward @ 0x080ca3d4：读 m_bingo.m_map30[CDataManager+0xb568+0x30
// =+0xb598] 的 key=1（单线奖励配置 <type, value>），按 type 构造 Inven_Item 后
// 以系统邮件发放。
// [SweepBingoEventLink4] 已闭合（AE 143/143，原 DIFF 144/145）：count 由
// unsigned int 改 int ⇒ 复现 ORIG 0x080ca5cd `mov -0x10(%ebp),%edx`（压栈前
// 提前装载 count，与形参 unsigned int 异型触发 4.4 同形）；unsigned 版 4.4
// 恒延迟到 `mov %eax,0x8(%esp)` 前装载（P211 档③/P75 反例，探针实测）。
void BingoEvent::sendOneMatchLineReward(CUser& user)
{
    // [b28d-D9] P167 差集（缺 G_CDataManager ×1）：ORIG 0x080ca3d4 无 rewardMap
    // 引用缓存槽，find 与 end 各自重新内联 `call G_CDataManager; lea 0xb598(%eax)`
    // （P0/P183）；具名引用使第二次 call 被槽缓存吞掉。
    std::map<int, std::pair<int, int> >::iterator it =
        G_CDataManager()->m_bingo.m_map30.find(1);
    if (it == G_CDataManager()->m_bingo.m_map30.end())
    {
        LogManager::logFormat(1, "BingoEvent.cpp",
                              __PRETTY_FUNCTION__, 0xc0,
                              "bingo script error");
        return;
    }

    const int type = it->second.first;
    const int value = it->second.second;

    Inven_Item item;
    // int（非 unsigned）：与形参 unsigned int 异型 ⇒ 4.4 提前装载（见函数头）。
    int count = 0;
    if (type == 0)
    {
        count = value;
    }
    else if (type == 1)
    {
        item.m_addInfo = 1;                                  // +0x02 itemIdx=1
        item.m_field1 = 2;                                   // +0x01 ITEM_TYPE=2
        item.m_addInfo2 = value;                             // +0x07 add_info
        item.m_fieldb = 1;                                   // +0x0b endurance
        item.ResetItemAttr();                                // +0x06 attr=0
    }
    else
    {
        CItem* found = G_CDataManager()->find_item(type);
        if (found == 0)
        {
            return;
        }
        item.m_addInfo = type;                               // +0x02 itemIdx
        found->make_item(item);                              // vtbl slot +0x08
        item.set_add_info(value);                            // m_addInfo2 = value
    }

    char sender[21];
    strncpy(sender,
            g_scriptStringManager_.findString(4, "game_server_msg_292", 0), 0x14);
    char title[256];
    strncpy(title,
            g_scriptStringManager_.findString(4, "game_server_msg_293", 0), 0xff);

    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender, item, count,
        static_cast<unsigned int>(user.getCurCharacNo()),
        title, static_cast<int>(strlen(title)),
        0, user.GetServerGroup(), false, false);
}