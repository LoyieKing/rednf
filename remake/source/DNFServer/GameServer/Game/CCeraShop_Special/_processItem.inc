// ============================================================================
// _processItem —— ORIG 0x08325b22 (T)
// 一般可叠/装备商品主路径：ProcessSpecialItem 扣费分发（c1=c2=0、flag2=false、
// extra=item.m_addInfo2）→ AvatarCoin::BuyProcess → insertItemIntoInventory(0xf)
// → SendUpdateItemList（artifact 走 ITEMSPACE 7）→ save 时 SaveInventory；
// 入包失败走 ReqDBSendNewMailCashShop 邮件兜底并返回 0。
// ============================================================================
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] P169 外层正向 if + goto 尾标签（return slot 直落 epilogue）
int CCeraShop::_processItem(CUser* user, Inven_Item item, int count, int& restrictCode,
                            long curSec, bool flag, bool save,
                            std::vector<Inven_Item>& items,
                            stCeraShopItemParam_t& param)
{
    // b28d-D42 verdict=DIFF [N93附] ORIG 0x08325b22 orig/ours=147/146 ndiff=8。
    // 残差仅尾部双 return 块发射序相反（ORIG ret0 块在 ret-slot 块之前；
    // N93附 四形探针全败）；语义等价封顶。
    int slot = -1;
    if (!ProcessSpecialItem(user, item.m_addInfo, count, restrictCode, curSec,
                            flag, items, param, 0, 0, false, item.m_addInfo2))
    {
        if (AvatarCoin::BuyProcess(user, item))
            return 0;

        if ((slot = user->getCurCharacInvenW()->insertItemIntoInventory(
                 item, (eItemAddReason)0xf, true, false)) != -1)
        {
            if (item.m_field1 == Inven_Item::ITEM_TYPE_ARTIFACT)
                user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)7,
                                         slot);
            else
                user->SendUpdateItemList(CUser::eSendTarget_1, ENUM_ITEMSPACE_0,
                                         slot);
            if (save)
                user->SaveInventory();
        }
        else
        {
            const char* msg =
                g_scriptStringManager_.findString(4, "game_server_msg_104",
                                                  (bool*)0);
            WongWork::CMailBoxHelper::ReqDBSendNewMailCashShop(
                user, item, 0, (unsigned int)user->getCurCharacNo(), true, msg,
                (int)std::strlen(msg));
            slot = 0;
        }
        goto end;
    }
    return 0;
end:
    return slot;
}

