// ---------------------------------------------------------------------------
// _processCoinGift —— ORIG T 0x08326332 (0x1a4)：五参重载（count 选用赠品种类）
// kindFlag：count==0xec→4、count==0xca→5、否则 0。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_CeraShopDeps] verdict=AE（DIFF→AE）。修法同四参重载：P28 声明序
// （ORIG 0x08326332 rep stos 在 ResetItemAttr 之后）+ 统计日志第 3/4 实参改
// item.m_addInfo(-0x54)/item.m_addInfo2(-0x4f)（原写 count/currencyType）。
void CCeraShop::_processCoinGift(CUser* user, unsigned short currencyType,
                                 unsigned int buyIdx, int cost, int count)
{
    Inven_Item item;
    item.m_addInfo = count;                           // movl param_5 → idx=count
    item.m_field1 = 2;                                // ITEM_TYPE_2
    item.m_addInfo2 = static_cast<int>(currencyType);
    item.m_fieldb = 2;
    item.ResetItemAttr();

    // [b28d-S278 实证] ORIG 0x08326332：msgBuf rep stos 在 ResetItemAttr（+0x041）
    // 之后（+0x046）⇒ item 先声明、msgBuf 后（P28）。
    char msgBuf[256] = {0};

    sprintf(msgBuf,
            g_scriptStringManager_.findString(4, "game_server_msg_02", (bool*)0),
            user->getCurCharacName());

    char kindFlag = '\0';
    if (count == 0xec)
    {
        kindFlag = '\x04';
    }
    else if (count == 0xca)
    {
        kindFlag = '\x05';
    }

    user->m_historyLog.SendMailForCeraShopGift(
        static_cast<unsigned int>(cost), kindFlag,
        static_cast<unsigned int>(count),
        static_cast<unsigned int>(currencyType));
    user->SendCeraShopGiftStatisticLog(buyIdx, static_cast<unsigned int>(cost),
                                       static_cast<long>(item.m_addInfo),
                                       static_cast<unsigned int>(item.m_addInfo2));

    CMailBoxHelper::ReqDBSendNewMailCashShop(user, item, 0u,
                                             static_cast<unsigned int>(cost), true,
                                             msgBuf, static_cast<int>(strlen(msgBuf)));
}