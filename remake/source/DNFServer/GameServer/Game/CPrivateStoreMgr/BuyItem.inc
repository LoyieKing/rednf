// ---- BuyItem（ORIG T 0x85c924c，报告 BuyItem.md 汇编 L15-401/反编译C L403-545 全文）----
// 物品入买方背包（avatar 走 mgr 所有权转移链），扣买方货款、安全上报与历史；
// slot/commissionBase/flag 三参在 ORIG 函数体内未被读取。
// [b28d-24] DIFF 残97: P167 MISSING getCurCharacName×2（真缺口）。ORIG 0x085c924c
// [b28d-solo-CPrivateStoreMgr] 已修真缺口：ORIG 共 4 次 getCurCharacName（trace 2
// 次 + historyLog 实参 2 次独立调用，P48）——删除 sellerName/buyerName 具名局部、
// trace 改全表达式临时链（P227，结果入 %esi/%ebx 无槽）、PrivateStoreItemAdd
// 实参改表达式直写。残 104 行 = N18a 单段 callee-saved 直落不可造 + 帧/槽（N82）。
// [L14] 本轮复核：385/381，6 块 = 帧 0xec vs 0xfc（+0x10，N115）+ 槽差 0x70
// + `mov 0x2(%eax),%edi` vs 我方 `mov 0x2(%eax),%eax; mov %eax,%edi`（N18a）⇒ 封顶。
// [K2 2026-09-11 复验] fresh .o：帧 ORIG 0xec vs 我方 0xfc（+0x10 = N115 4.4
// outgoing 预留）不变；15 组 slot-pair 以 +4×3 / -116×2 为主（多段 ⇒ P331 不
// 可修）；N18a 两段物化 `mov 0x2(%eax),%eax; mov %eax,%edi` 仍不可造 ⇒ 维持封顶。
int CPrivateStore::BuyItem(int /*slot*/, Inven_Item& item, int gold,
                           int /*commissionBase*/, int /*flag*/, int& outSlot)
{
    // [b28d-solo-L43] 类型统一 int 实测无效（三值仍 -0x34/-0x30/-0xac，ORIG 为
    // -0xa4/-0xa0/-0x9c 同区）⇒ 回退原型；属 N82 分区差异。
    unsigned int resealCount = (unsigned int)item.GetReSealCount();
    unsigned int upgrade = (unsigned int)item.GetUpgrade();
    // P227/P0：ORIG 在 ctor 前把两次 getCurCharacName 读入 %esi/%ebx 跨 ctor
    // 保活（无具名局部槽）⇒ 全表达式临时链 + 实参内联。
    cMyTrace(__PRETTY_FUNCTION__, 0xa63, 0)(
        "PrivateStore buy seller_charac_name(%s), buyer_charac_name(%s), "
        "item_id(%d), add_info(%d), upgrade(%d), reseal_count(%d), sell_price(%d)",
        m_pSeller->getCurCharacName(), m_pBuyer->getCurCharacName(),
        (int)item.m_addInfo, (int)item.m_addInfo2, upgrade, resealCount, gold);

    int slot;
    if ((unsigned char)item.m_itemType == 8)   // ITEM_TYPE_AVATAR
    {
        slot = m_pBuyer->getCurCharacInvenW()->tryInsertAvatarIntoInventory(item);
        if (slot >= 0)
        {
            // [L14/C07 试形（已回滚）] 具名 int 局部复现「字段读升格 callee-saved」
            // （ORIG `mov 0x7(%eax),%ebx` 跨两 call 保活）实测 **385→387 劣化**
            // （4.4 对具名局部落槽 +2）⇒ 本件非 C07 命中形（其 CDisjointer 件机理
            // 不同），维持 N18a/N82 封顶。
            // [b28d-G11 P183/P0] ORIG 0x085c93c0..0x085c93e3 段：GetAvatarItemMgrW
            // 的结果 `mov %eax,(%esp)` 直接作 EraseAvatarItem 的 this，全程**无**
            // `mov %eax,-0xNN(%ebp)` 中转（对比我方 -0x20 多 4B 槽 + 2 条指令）
            // ⇒ 原码是链式表达式，无具名指针缓存。
            // [A/B 复核 2026-09-10] 恢复具名 sellerMgr 形后 identical 仍 171/368/2/9
            // （无变化）⇒ 该件不是 identical −1 的来源，保留链式版（diff 110→92）。
            WongWork::Avatar_Item* erased =
                m_pSeller->getCurCharacInvenW()->GetAvatarItemMgrW()->EraseAvatarItem(
                    (int)item.m_addInfo2);
            if (erased == 0)
                return -1;

            slot = m_pBuyer->getCurCharacInvenW()->insertAvatarIntoSpecificSlot(
                item, slot, (eAvatarItemAddReason)0xb, false);
            // [BigStore P225] ORIG 0x085c94a0 `cmpl $-0x1,-0x24; sete %al;
            // test; je 续` 物化形 ⇒ while 复现（4.4 直跳无 sete）。
            while (slot == -1)
                return -1;

            m_pBuyer->getCurCharacInvenW()->GetAvatarItemMgrW()->AddAvatarItem(
                (int)item.m_addInfo2, erased);

            DB_AvatarChangeOwner::makeRequest(
                (unsigned int)item.m_addInfo2,
                (unsigned int)m_pSeller->getCurCharacNo(),
                (unsigned int)m_pBuyer->getCurCharacNo(),
                (unsigned char)(slot + 10));
            m_pBuyer->getCurCharacInvenR()->SendAvatarEvent(
                0x36, (int)item.m_addInfo2, 0, slot + 10, 0);
        }
    }
    else
    {
        slot = m_pBuyer->getCurCharacInvenW()->insertItemIntoInventory(
            item, (eItemAddReason)0x11, false, false);
    }

    if (slot < 0)
        return slot;

    // 本地条目：仅预写槽位号到 +0x48，再由 GetItemInfo 按 +0x46 区切回填完整数据。
    PrivateStoreItem storeEntry;
    storeEntry.m_field48 = (unsigned short)slot;
    GetItemInfo(m_pBuyer, storeEntry);

    int historyAddInfo;
    if (item.isEquipableItemType())
    {
        outSlot = 1;
        historyAddInfo = 1;
    }
    else
    {
        outSlot = (int)storeEntry.m_item.get_add_info();
        historyAddInfo = (int)item.get_add_info();
    }

    // P48：ORIG 此处重新调用两次 getCurCharacName（0x085c93xx 段，共 4 次），
    // 不复用 trace 的局部；4.4 无 CSE ⇒ 实参表达式直写。
    m_pBuyer->m_historyLog.PrivateStoreItemAdd(
        (INVEN_TYPE)1, item, outSlot, historyAddInfo, (eItemAddReason)0x11,
        m_pSeller->getCurCharacName(), m_pBuyer->getCurCharacName());

    // XNuclear::CHades::GetIP @0x84b9774（CUser.h 已声明，直接调用与 ORIG 字节一致）
    char ipBuf[16];
    m_pSeller->getHades()->GetIP(ipBuf);

    unsigned int buyerMoney =
        (unsigned int)m_pBuyer->getCurCharacInvenR()->get_money();
    // Secu_AccountHacking::sendGold @0x8278cce（CHackAnalyzer.h 已声明，
    // 直接调用与 ORIG 字节一致；参数 (0, sellerId, gold, buyerMoney, 1, ip)）。
    m_pBuyer->getAcccountHackInfo()->sendGold(
        0, (unsigned int)m_pSeller->get_acc_id(), (unsigned int)gold,
        buyerMoney, 1, ipBuf);

    m_pBuyer->getCurCharacInvenW()->use_money(gold, (eMoneySubReason)0xe, true);

    HistoryLog::WriteBuyItem((FILE*)m_pBuyer->m_historyLogFp,   // +0x796f8 句柄（int→FILE* 直转，先例 CGameManager.cpp:2727）
                             (int)item.m_addInfo, (int)item.m_addInfo2);
    return slot;
}