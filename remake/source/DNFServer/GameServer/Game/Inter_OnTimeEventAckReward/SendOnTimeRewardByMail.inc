// ---------------------------------------------------------------------------
// Inter_OnTimeEventAckReward::SendOnTimeRewardByMail(uint, uint, uint)
//   ORIG 0x084e1578（T，size 0x17c）。非虚。nCharacNo=0xc 槽（邮件接收角色号）、
//   itemNo=0x10 槽（find_item 参数）、count=0x14 槽（create_item 数量）。
//   Inven_Item 构造在 find_item 判空之后（ORIG +0x02b）；item.m_addInfo(+0x02)
//   即 Ghidra 的 local_60@-0x5c（create_item 后仍读该字段）。
// ---------------------------------------------------------------------------
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
}