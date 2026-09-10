void CCeraShop::_processItemGift(CUser* user, Inven_Item item,
                                 unsigned int buyIdx, int giftKind)
{
    // [b28d-L14 实证] ORIG 0x083260c2：rep stos 零初始化 + getCurCharacName 作
    // sprintf 第三实参内联，结果经 %ebx 跨 findString call 保活（P52/P214③）。
    char msgBuf[256] = {0};
    sprintf(msgBuf,
            g_scriptStringManager_.findString(4, "game_server_msg_02", (bool*)0),
            user->getCurCharacName());

    user->m_historyLog.SendMailForCeraShopGift(
        static_cast<unsigned int>(giftKind), '\0',
        static_cast<unsigned int>(item.m_addInfo),
        static_cast<unsigned int>(item.m_addInfo2));
    user->SendCeraShopGiftStatisticLog(buyIdx, static_cast<unsigned int>(giftKind),
                                       static_cast<long>(item.m_addInfo),
                                       static_cast<unsigned int>(item.m_addInfo2));

    CMailBoxHelper::ReqDBSendNewMailCashShop(user, item, 0u,
                                             static_cast<unsigned int>(giftKind), true,
                                             msgBuf, static_cast<int>(strlen(msgBuf)));
