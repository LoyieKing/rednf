int CEventActionMng::process_gold_mail_send(CUser* user, Action_SendMail& mail)
{
    // [b28d-solo-EventClassify_Action] ORIG 0x0810d83c，139=139 AE。
    //   旧判词记的 getCurCharacNo eax→edx 中转系旧 .o 假象；fresh 编译后
    //   尾段 `mov 0x10(%ebp),%edx; mov 0x28(%edx),%edx` 与 ORIG 全等。
    Inven_Item item;
    item.m_addInfo = -1;

    char title[256] = {0};
    char sender[30] = {0};
    std::strncpy(sender, g_scriptStringManager_.findString(
                     4, toTString(mail.m_field30), 0), 0x1d);
    std::strncpy(title, g_scriptStringManager_.findString(
                     4, toTString(mail.m_field34), 0), 0xff);

    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender, item,
        (unsigned int)mail.m_gold,
        (unsigned int)user->getCurCharacNo(),
        title, (int)std::strlen(title),
        (unsigned int)(signed char)mail.m_coin,
        user->GetServerGroup(), false, false);
    return 0;
