// ORIG T _ZN27Inter_VillageAttackedReward17_send_reward_itemEP5CUsermii
// @0x084e0390 sz0x232（docs/class_func_reports/Inter_VillageAttackedReward/
// _send_reward_item.md 全文已读）。按 itemIdx 现做道具经系统邮件发放：
// -1 → 错误日志直返；1 → 耐久字段 m_fieldb 置 1；第 4 参 ORIG 未消费。
void Inter_VillageAttackedReward::_send_reward_item(CUser* user,
                                                    itemIndexOnlyServer_t itemIdx,
                                                    int count, int rank)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 151/151：content 声明提前于 title
    // （ORIG 先清 -0x173 后清 -0x36 ⇒ 声明序=清零序，P214③）。
    (void)rank;
    Inven_Item item;
    item.m_addInfo = static_cast<int>(itemIdx);
    G_CDataManager()->m_itemList->create_item(
        ENUM_ITEM_CREATE_TYPE_0, item, count);
    if (item.m_addInfo == -1)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x4dbc,
            "VillageAttackedReward Invalid Item Index [item:%u]",
            item.m_addInfo);
        return;
    }
    if (item.m_addInfo == 1)
    {
        item.m_fieldb = 1;
    }
    // [b28d-63] ORIG 无 memset 调用（p167 EXTRA ×2）⇒ 聚合零初始化（P214③）。
    char content[256] = {0};
    char title[30] = {0};   // [b28d-solo-Inter_LoadEtc_impl] ORIG 先清 content（-0x173）
                            // 后清 title（-0x36）⇒ content 声明在前（声明序=清零序）。
    strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_124", 0),
            0x1d);
    strncpy(content, g_scriptStringManager_.findString(4, "game_server_msg_125", 0),
            0xff);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, item, 0, static_cast<unsigned int>(user->getCurCharacNo()), content,
        static_cast<int>(strlen(content)), 0xc, user->GetServerGroup(), false,
        false);
}