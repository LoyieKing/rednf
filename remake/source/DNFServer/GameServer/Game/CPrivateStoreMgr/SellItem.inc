// ---- SellItem（ORIG T 0x85c97e2，报告 SellItem.md 汇编 L15-274/反编译C L276-419 全文）----
// 净额入卖方：无 doll 直接 gain_money；有 doll 组邮件文本走系统邮件。
// slot 参数未读；恒返回 1。
// [b28d-24] DIFF 残109: P167 EXTRA memset×2（ORIG 无 memset 调用）。ORIG 0x085c97e2
// [b28d-solo-CPrivateStoreMgr] **AE（clean 后 0 残差，255=255）**：EXTRA memset×2
// → `char msgBuf[256]={0}; char title[21]={0};`（P214③/P202①）；三处条件极性/块序
// 按 ORIG 正向化；**真语义 bug 修正**：msg_184/msg_185 的 snprintf 变参错位
//（count 属 msg_185 分支，0x085c99a8/0x085c9a1f 压栈槽证据）。
int CPrivateStore::SellItem(int /*slot*/, Inven_Item& item, int gold,
                            int commission, int count, int field7)
{
    int netGold = gold - commission;
    // ORIG 0x085c97xx：`call isEquipableItemType; test %al,%al; je <else>` 正向形
    //（then = 装备分支），`!f()` 写法会多一条 xor 并反转块序。
    if (item.isEquipableItemType())
    {
        field7 = 0;
        count = 1;
    }
    else
    {
        field7 -= count;
    }

    m_pSeller->m_historyLog.PrivateStoreItemDel(
        (INVEN_TYPE)1, item, count, field7, (eItemDelReason)0x16,
        m_pSeller->getCurCharacName(), m_pBuyer->getCurCharacName());

    // ORIG 0x085c98xx：`call isDollCreated; test %al,%al; je <gain_money 块>`
    // 正向形（then = 邮件分支）；`!f()` 写法多 xor 且块序反。
    if (isDollCreated())
    {
        // P214③/P202①：ORIG 无 memset call（EXTRA×2），零初始化列表即 rep stos/
        // 双写形；0x100 / 0x15 字节。
        char msgBuf[256] = {0};
        char title[21] = {0};

        std::strncpy(title,
                     g_scriptStringManager_.findString(4, "game_server_msg_183", 0),
                     0x14);

        CItem* pItem = G_CDataManager()->find_item((int)item.m_addInfo);
        // ORIG 0x085c99xx `call is_stackable; xor $1; test; je` ⇒ 否定条件 + 非堆叠
        // 分支 fall-through（msg_184）；正向写法少 xor 且块序反。
        // [b28d-solo-CPrivateStoreMgr] 真语义 bug 修正：ORIG 0x085c99a8（!stackable
        // fall-through, msg_184）变参 5 个 `GetItemName,gold,commission,GetItemName,
        // netGold`；0x085c9a1f（stackable, msg_185）变参 6 个含 count（0x1c(%ebp)
        // 压 0x10(%esp)）。旧码把 count 放在 184 分支 ⇒ 邮件正文参数错位。
        if (!pItem->is_stackable())
        {
            OS_API::snprintf(msgBuf, 0xff,
                             g_scriptStringManager_.findString(4, "game_server_msg_184", 0),
                             pItem->GetItemName(), gold, commission,
                             pItem->GetItemName(), netGold);
        }
        else
        {
            // ORIG 对 GetItemName 连续两次独立调用，保形转写。
            OS_API::snprintf(msgBuf, 0xff,
                             g_scriptStringManager_.findString(4, "game_server_msg_185", 0),
                             pItem->GetItemName(), count, gold, commission,
                             pItem->GetItemName(), netGold);
        }
        Inven_Item mailItem;   // ORIG 默认构造 W 0x80cb854
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, mailItem, (unsigned int)netGold,
            (unsigned int)m_pSeller->getCurCharacNo(),
            msgBuf, (int)std::strlen(msgBuf), 0u,
            m_pSeller->GetServerGroup(), false, false);
    }
    else
    {
        m_pSeller->getCurCharacInvenW()->gain_money(
            netGold, (eMoneyAddReason)0xb, true, 0);
    }
    HistoryLog::WriteSellItem((FILE*)m_pSeller->m_historyLogFp,
                              (int)item.m_addInfo, (int)item.m_addInfo2);
    return 1;
}