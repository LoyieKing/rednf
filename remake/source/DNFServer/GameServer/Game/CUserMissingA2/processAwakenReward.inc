void CUser::processAwakenReward(char stage)
{
    unsigned int rewardItemId = 0x3c0;
    int sealCount = 1;
    char mailMsg[256];
    char mailTitle[21];
    switch (stage)
    {
    case 0:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_1", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_1", 0), 0x14);
        break;
    case 1:
    case 7:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_2", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_2", 0), 0x14);
        break;
    case 2:
    case 5:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_3", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_3", 0), 0x14);
        break;
    case 3:
    case 8:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_4", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_4", 0), 0x14);
        break;
    case 4:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_5", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_5", 0), 0x14);
        break;
    case 6:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_6", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_6", 0), 0x14);
        break;
    default:
        return;
    }

    Inven_Item item;
    CItem* def = G_CDataManager()->find_item(rewardItemId);
    if (def == 0)
        return;
    item.m_addInfo = rewardItemId;
    def->make_item(item);
    item.set_add_info(sealCount);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        mailTitle, item, 0, getCurCharacNo(), mailMsg, (int)strlen(mailMsg),
        12, GetServerGroup(), false, false);
