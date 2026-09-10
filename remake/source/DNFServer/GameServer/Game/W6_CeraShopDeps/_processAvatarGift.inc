void CCeraShop::_processAvatarGift(CUser* user, int slot, int periodDays, char c1,
                                   char c2, char const* agencyName,
                                   unsigned int buyIdx, int cost)
{
    int deadline = 0;
    if (periodDays != 0 && periodDays != 0x16d)
    {
        deadline = GlobalData::s_systemTime_.getCurSec() + periodDays * 0x15180;
    }

    // [b28d-L14 实证] ORIG 0x08326036：msgBuf 声明在 deadline 计算之后
    // （rep stos 0x40 dwords），getCurCharacName 内联进 sprintf 实参（P52）。
    char msgBuf[256] = {0};
    sprintf(msgBuf,
            g_scriptStringManager_.findString(4, "game_server_msg_02", (bool*)0),
            user->getCurCharacName());

    user->m_historyLog.SendMailForCeraShopGift(
        static_cast<unsigned int>(cost), '\x02', static_cast<unsigned int>(slot), 1u);
    user->SendCeraShopGiftStatisticLog(buyIdx, static_cast<unsigned int>(cost),
                                       static_cast<long>(slot), 1u);

    CMailBoxHelper::ReqDBSendNewAvatarMailCashShop(
        user, cost, slot, deadline, c1, c2, agencyName, msgBuf,
        static_cast<int>(strlen(msgBuf)));
