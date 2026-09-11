// ---------------------------------------------------------------------------
// _processCreatureGift —— ORIG T 0x083265e8 (0x151)
// 宠物赠送：ITEM_TYPE_CREATURE_EQUIP(5) 走 ReqDBSendNewCreatureMail，
// 其余走 ReqDBSendNewMailCashShop。
// ---------------------------------------------------------------------------
void CCeraShop::_processCreatureGift(CUser* user, Inven_Item item, unsigned int buyIdx,
                                     int a4, int a5, int a6)
{
    (void)a5;   // ORIG 栈参存在但本函数未消费（对照报告完整指令清单）
    (void)a6;

    // [b28d-L14 实证] ORIG 0x083265e8 同 rep stos 零初始化 + 内联实参形（P52）。
    char msgBuf[256] = {0};
    sprintf(msgBuf,
            g_scriptStringManager_.findString(4, "game_server_msg_02", (bool*)0),
            user->getCurCharacName());

    user->m_historyLog.SendMailForCeraShopGift(
        static_cast<unsigned int>(a4), '\x03',
        static_cast<unsigned int>(item.m_addInfo),
        static_cast<unsigned int>(item.m_addInfo2));
    user->SendCeraShopGiftStatisticLog(buyIdx, static_cast<unsigned int>(a4),
                                       static_cast<long>(item.m_addInfo),
                                       static_cast<unsigned int>(item.m_addInfo2));

    if (static_cast<unsigned int>(item.m_field1) == 5u)
    {
        CMailBoxHelper::ReqDBSendNewCreatureMail(user, a4, item, msgBuf,
                                                 static_cast<int>(strlen(msgBuf)));
    }
    else
    {
        CMailBoxHelper::ReqDBSendNewMailCashShop(user, item, 0u,
                                                 static_cast<unsigned int>(a4), true,
                                                 msgBuf, static_cast<int>(strlen(msgBuf)));
    }
}