// gift 族（coinGift×2/itemGift/avatarGift/creatureGift）ORIG 均内联
// memset 0x100（rep stos 慢路）+ getCurCharacName/findString/sprintf 三调用，
// 本批删除 FormatGiftMessage/GiftHistoryLog helper（P49/§4.1 发明符号），逐
// 函数内联复刻 ⇒ P167 差集归零；coinGift 族余 gap≈N78 rep 段（28 vs 5 insn）。
// ---------------------------------------------------------------------------
// _processCoinGift —— ORIG T 0x0832619c (0x195)：四参重载
// 代币物品 idx=1、count=currencyType 构造 Inven_Item → 赠送邮件 + 统计日志。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_CeraShopDeps] verdict=AE（DIFF→AE）。修法：①P28 声明序——item 先、
// msgBuf 后（ORIG 0x0832619c rep stos 在 ResetItemAttr call +0x041 之后 +0x046）；
// ②真 bug：SendCeraShopGiftStatisticLog 第 3/4 实参应取 item.m_addInfo(-0x43) /
// item.m_addInfo2(-0x3e)，原写常量 1L/currencyType（数据源取错，P92 族）。
void CCeraShop::_processCoinGift(CUser* user, unsigned short currencyType,
                                 unsigned int buyIdx, int cost)
{
    Inven_Item item;
    item.m_addInfo = 1;                               // movl $1 → idx=1
    item.m_itemType = 2;                                // ITEM_TYPE_2（movb $2,-0x44）
    item.m_addInfo2 = static_cast<int>(currencyType); // movzwl → set_add_info2
    item.m_endurance = 2;                                // endurance=2
    item.ResetItemAttr();

    // [b28d-S278 实证] ORIG 0x0832619c：msgBuf 的 rep stos 在 ResetItemAttr call
    // （+0x041）之后（+0x046）⇒ 声明序为 item 先、msgBuf 后（P28 声明序即槽序）。
    char msgBuf[256] = {0};

    sprintf(msgBuf,
            g_scriptStringManager_.findString(4, "game_server_msg_02", (bool*)0),
            user->getCurCharacName());

    user->m_historyLog.SendMailForCeraShopGift(
        static_cast<unsigned int>(cost), '\x01', 1u,
        static_cast<unsigned int>(currencyType));
    // [b28d-S278 真 bug 修正] ORIG +0x11b：统计额取 item.m_addInfo(-0x43) 与
    // item.m_addInfo2(-0x3e)，原写常量 1/currencyType 取错数据源（P92 族）。
    user->SendCeraShopGiftStatisticLog(buyIdx, static_cast<unsigned int>(cost),
                                       static_cast<long>(item.m_addInfo),
                                       static_cast<unsigned int>(item.m_addInfo2));

    CMailBoxHelper::ReqDBSendNewMailCashShop(user, item, 0u,
                                             static_cast<unsigned int>(cost), true,
                                             msgBuf, static_cast<int>(strlen(msgBuf)));
}