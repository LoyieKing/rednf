// ORIG 0x0816471e：RDAR 字符串取标题/正文，附道具（idx 0x28ec3d @ m_addInfo，
// add_info = m_fieldC @ m_addInfo2）经 ReqDBSendNewSystemMail 发系统邮件。
void CEventCeraShopRewardPoint::SendReward(CUser* user)
{
    if (user == 0)
        return;

    char titleBuf[30];
    strncpy(titleBuf,
            g_scriptStringManager_.findString(4, "tw_shop_open_title", (bool*)0),
            0x1d);

    char contentBuf[256];
    strncpy(contentBuf,
            g_scriptStringManager_.findString(4, "tw_shop_open_content", (bool*)0),
            0xff);

    Inven_Item item;
    item.m_addInfo = 0x28ec3d;   // item idx @ +0x02
    item.m_addInfo2 = m_fieldC;  // add_info @ +0x07
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        titleBuf, item, (unsigned int)m_fieldC,
        (unsigned int)user->get_charac_no(-1),
        contentBuf, (int)strlen(contentBuf), 0,
        (ENUM_SERVER_GROUP)user->GetServerGroup(), false, false);
}