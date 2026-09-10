int CEventActionMng::process_coin_send(CUser* user, Action_SendMail& mail,
                                       Inven_Item& item)
{
    if (mail.m_field1c != 0)
    {
        user->getCurCharacInvenW()->SetEventCoin(
            user->getCurCharacInvenR()->GetEventCoin() + item.m_addInfo2);
        user->m_historyLog.EventCoinAdd(
            user->getCurCharacInvenR()->GetEventCoin(), item.m_addInfo2,
            (eCoinAddReason)2);
        user->SaveMoneyCoin();
        user->SendCoin();
    }
    else
    {
        item.m_fieldb = 1;                       // +0x0b word = 1
        char title[256] = {0};
        char sender[30] = {0};
        std::strncpy(sender, g_scriptStringManager_.findString(
                         4, toTString(mail.m_field30), 0), 0x1d);
        std::strncpy(title, g_scriptStringManager_.findString(
                         4, toTString(mail.m_field34), 0), 0xff);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            sender, item,
            (unsigned int)mail.m_gold,                       // +0x28
            (unsigned int)user->getCurCharacNo(),
            title, (int)std::strlen(title),
            (unsigned int)(signed char)mail.m_coin,          // +0x2c 低字节
            user->GetServerGroup(), false, false);
    }
    return 0;
