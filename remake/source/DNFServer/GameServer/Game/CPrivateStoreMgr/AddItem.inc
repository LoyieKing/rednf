// ---- AddItem（ORIG T 0x85c6d48，报告 AddItem.md）----
// ISeller 虚表槽 +0x10。卖家上架：doll 存在时校验 doll 物品与槽位上限；
// 校验背包来源物品（非现金价、非化身、可交易限制），prevCountSlot==0 时重置
// 全部条目后按 slot_no 落位。
// [b28d-24] DIFF 残87: P167 MISSING vector::operator[]×2（访问方式差异）。ORIG 0x085c6d48
// [b28d-solo-CPrivateStoreMgr] P167 归零（尾部 op[]×3 + +0x3d 字段直读；
// invenType 内联消 -0x1c 槽）。残 41 行 = find_item/GetCashPrice 的 sete/setg
// 物化点位置（P225 while 形已应用）+ N7 寄存器角色互换（invenType eax↔edx）+
// 帧/槽（N82）。
// [L14] 本轮复核：302/301，仅 1 差异块（6 行）= ORIG 0x085c6d48+0x147
// `mov %eax,%edx; lea -0x59(%ebp),%eax; mov %edx,0x8(%esp) … mov %eax,(%esp)`
// vs 我方 eax↔edx 角色互换（长度全等）⇒ N7 封顶。
// [K2 2026-09-11 复验] fresh .o：fn_diff 155 行中实质 slot-pair 仅 2 组（+63/0），
// 实体差异仍为唯一块 = GetInvenSlot 调用点 `mov %eax,%edx; lea -0x59(%ebp),%eax`
// vs 我方 `lea -0x59(%ebp),%edx; mov %eax,0x8(%esp)`（长度全等，N7：4.1 让 sret
// lea 占 %eax / invenType 值落 %edx）⇒ 维持 N7 封顶。
bool CPrivateStore::AddItem(PrivateStoreItem& item, int prevCountSlot)
{
    if (isDollCreated())
    {
        CItem* dollInfo =
            G_CDataManager()->find_item((int)GetDollItemID());
        if (dollInfo == 0)
        {
            LogManager::logFormat(
                1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x550,
                "CPrivateStore::AddItem ERROR : shop_doll_item_id(%d)",
                (unsigned int)GetDollItemID());
            return false;
        }
        STExpertJobScript* expertScript = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(4);  // 头返回 void*
        int maxSlot = (int)expertScript->GetShopDollSlot(
            (int)static_cast<CStackableItem*>(dollInfo)->m_int32c);   // ORIG 读 doll 物品 +0x32c
        if (maxSlot < 1)
        {
            LogManager::logFormat(
                1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x559,
                "CPrivateStore::AddItem ERROR : shop_doll_item_index(%d)",
                (int)static_cast<CStackableItem*>(dollInfo)->m_int32c);
            return false;
        }
        if (maxSlot <= (int)item.m_field44)
        {
            LogManager::logFormat(
                1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x55f,
                "CPrivateStore::AddItem ERROR : slot_no(%d), shop_doll_item_index(%d), maxSlot(%d)",
                (int)item.m_field44, (int)static_cast<CStackableItem*>(dollInfo)->m_int32c, maxSlot);
            return false;
        }
    }

    if (item.m_field44 >= 7)
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x566,
                              "CPrivateStore::AddItem slot_no(%d) ERROR",
                              (int)item.m_field44);
        return false;
    }

    if (m_pSeller == 0)
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x56c,
                              "CPrivateStore::AddItem seller_ == NULL ERROR");
        return false;
    }
    if (m_pSeller->getCurCharacR() == 0)
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x572,
                              "CPrivateStore::AddItem getCurCharacR() == NULL ERROR");
        return false;
    }
    const CInventory* inventory = m_pSeller->getCurCharacInvenR();
    if (inventory == 0)
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x579,
                              "CPrivateStore::AddItem inventory == NULL ERROR");
        return false;
    }

    // [b28d-solo-CPrivateStoreMgr] P0：ORIG 0x085c6ff1 后 `mov %eax,%edx`（无槽）
    // ⇒ invenType 是内联右值，无具名局部（旧具名形多 -0x1c 槽并使 srcItem 槽位
    // 由 -0x59 漂到 -0x5d）。
    Inven_Item srcItem = inventory->GetInvenSlot(
        (INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)item.m_field46),
        (int)item.m_field48);
    // [b28d-solo-CPrivateStoreMgr] P197③ 预清零两句式 + P225 while 形：
    // ORIG 0x085c7015 `movl $0x0,-0x18` 预清零；0x085c7035 `cmpl $0,-0x18;
    // sete %al; test; je` 槽比零物化 ⇒ while(==0) 复现。
    CItem* itemInfo = 0;
    itemInfo = G_CDataManager()->find_item((int)srcItem.m_addInfo);
    while (itemInfo == 0)
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x582,
                              "CPrivateStore::AddItem find_item() ERROR item_id(%d)",
                              (int)srcItem.m_addInfo);
        return false;
    }

    // ORIG：cash 价 >=1 的商城物品禁止上架（报告反编译 iVar6<1 才继续）。
    // ORIG 0x085c7088 `test %eax,%eax; setg %al; test %al,%al; je` ⇒ while 形。
    while ((int)itemInfo->GetCashPrice() >= 1)
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x5b8,
                              "CPrivateStore::AddItem GetCashPrice() ERROR item_id(%d), cash_price(%d)",
                              (int)srcItem.m_addInfo, (int)itemInfo->GetCashPrice());
        return false;
    }
    // ORIG 经 CItem vtable 槽 +0x10 判定化身类道具，禁止私商上架。
    // [b28d-solo] ORIG 块序：IsAvatarItem（0x85c70dd 虚调段）在 IsTradeRestriction
    // （0x85c71xx lea 副本+0x33 段）之前 ⇒ 两块顺序照 ORIG 排列。
    if (itemInfo->IsAvatarItem())
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x5be,
                              "CPrivateStore::AddItem IsAvatarItem() ERROR item_id(%d), cash_price(%d)",
                              (int)srcItem.m_addInfo, (int)itemInfo->GetCashPrice());
        return false;
    }

    // [b28d-solo-CPrivateStoreMgr] P59 负向守卫形：ORIG `call IsTradeRestriction;
    // test %al,%al; setne %al; test %al,%al; je <主流程>`（restriction 为真直返）
    // ⇒ 旧 `if(!f()){主流程}` 形产 sete 且块序反。
    if (srcItem.m_upgradeSep.IsTradeRestriction())   // ORIG lea 副本+0x33（UpgradeSeparateInfo 子对象）
        return false;

    // [b28d-PS] ORIG 0x085c7160..0x085c7199：两组边界各自独立 fail 块
    // （`jg fail; …; test; jns next; fail: mov$0,jmp`）⇒ 原码为 `||` 短路
    // 两条独立 return false（&& 链在 4.4 会共享末尾 fail 并少 2 个块）。
    if (item.m_field44 >= 0xe || item.m_field44 < 0)
        return false;
    if (prevCountSlot >= 0xe || prevCountSlot < 0)
        return false;

    if (prevCountSlot == 0)
        ResetItems();

    // [b28d-solo-CPrivateStoreMgr] P157/P183 + P167：ORIG 尾部逐次
    // operator[]×3（读 +0x3d 字段 / Reset / operator=），且 +0x3d 判定
    // 是**直接字段读**非 GetState() 调用（旧缓存引用 ⇒ MISSING op[]×2
    // + EXTRA GetState×1）。
    if (m_items[item.m_field44].m_flag3d != 0)
        return false;

    if (item.m_field46 == 3)
        return false;   // ORIG：item space 3 的条目不可上架

    item.m_flag3d = 1;
    m_items[item.m_field44].Reset();
    m_items[item.m_field44] = item;
    return true;
}