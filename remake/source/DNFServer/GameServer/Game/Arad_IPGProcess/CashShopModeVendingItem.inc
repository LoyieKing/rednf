// ---------------------------------------------------------------------------
// CashShopModeVendingItem —— ORIG T 0x0819c862（报告 CashShopModeVendingItem.md）。
// 非特殊商品：insertItemIntoInventory 直插背包（失败转邮件补发）；
// 特殊商品：ProcessSpecialItem 通道。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] AE + 两处真语义 bug：①IsSpecialItem 第 1 实参按 ORIG
// 0x0819c8dd 从 localItem.m_addInfo(-0x58) 读（原写形参 itemIdx）；②日志条件极性反置
// （ORIG 0x0819cacc `test %al,%al; je end` ⇒ result!=0 才打日志，原写 !result）；
// ③删具名 ceraShop/result（P0）后槽位全对齐：localItem -0x5a / item -0x14 /
// outType -0x18 / outValue -0x1c / special -0x1d / msg -0xc / slot -0x10，
// strlen 结果 %ebx 跨 getCurCharacNo 保活。
void Arad_IPGProcess::CashShopModeVendingItem(
    CUser* user, itemIndexOnlyServer_t itemIdx, unsigned short stack, int count,
    long seq, std::vector<Inven_Item>& out,
    WongWork::CCeraShop::stCeraShopItemParam_t& param, int& restrictOut,
    int characNo, char buyFlag)
{
    CItem* item = G_CDataManager()->find_item((int)itemIdx);
    if (item == 0)
        return;

    Inven_Item localItem;
    item->make_item(localItem);
    localItem.m_addInfo = (int)itemIdx;   // ORIG -0x58 = item +2
    localItem.m_addInfo2 = (int)stack;    // ORIG -0x53 = item +7

    int outType = 0;
    int outValue = 0;
    bool special = false;
    // ORIG 0x0819c8dd：IsSpecialItem 第 1 实参从 localItem.m_addInfo(-0x58) 读，非形参槽；
    // m_vm 无具名局部（每次 `mov 0x8(%ebp),%eax; mov (%eax),%eax` 重取）。
    if (!reinterpret_cast<WongWork::CCeraShop*>(m_vm)->IsSpecialItem(
            localItem.m_addInfo, outType, outValue, special))
    {
        int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            localItem, (eItemAddReason)0x1e, true, false);
        if (slot != -1)
        {
            user->SendUpdateItemList(CUser::eSendTarget_1,
                                     (ENUM_ITEMSPACE)0, slot);
            user->SaveInventory();
        }
        else
        {
            const char* msg =
                g_scriptStringManager_.findString(4, "game_server_msg_104", 0);
            WongWork::CMailBoxHelper::ReqDBSendNewMailCashShop(
                user, localItem, 0, (unsigned int)user->getCurCharacNo(), true, msg,
                (int)strlen(msg));
            slot = 0;
        }
    }
    else
    {
        // ORIG 0x0819cacc：`call; test %al,%al; je end`（result 无槽）⇒ 条件内联。
        // 极性：result!=0 才打日志（原 !result 写反）。
        if (reinterpret_cast<WongWork::CCeraShop*>(m_vm)->ProcessSpecialItem(
                user, (int)itemIdx, count, restrictOut, seq, true, out, param,
                0, 0, false, 1))
        {
            LogManager::logFormat(1, "localjapan/Arad_IPGProcess.cpp",
                                  __PRETTY_FUNCTION__, 0x1a2,
                                  "ProcessSpecialItem failed");
        }
    }
}