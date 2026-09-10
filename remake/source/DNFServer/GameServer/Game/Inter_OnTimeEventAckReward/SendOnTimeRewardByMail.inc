void Inter_OnTimeEventAckReward::SendOnTimeRewardByMail(unsigned int nCharacNo, unsigned int itemNo,
                                                        unsigned int count)
{
    char body[256];
    char title[21];
    int local_10;

    local_10 = (int)G_CDataManager()->find_item((int)itemNo);
    if (local_10 == 0) {
        return;
    }
    Inven_Item item;
    item.m_addInfo = (int)itemNo;
    G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, item, (int)count);
    if (item.m_addInfo != 0 || item.get_add_info() != 0) {
        strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_132", 0), 0x14);
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_134", 0), 0xff);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, item, 0, nCharacNo, body, (int)strlen(body), 0,
            (ENUM_SERVER_GROUP)0, false, false);
        item.reset();
    }
    return;
