// ---- RecallPrivateStoreItemToMail（ORIG T 0x85c5cec，报告 RecallPrivateStoreItemToMail.md）----
// 收集 state==3 且物品索引非 0 的条目，经 ReqDBSendNewSystemMultiMail 以系统邮件退还；
// 发送失败仅 trace；成功或无条目时 ResetItems。恒返回 1。
// [b28d-24] DIFF 残147: P167 EXTRA vector::operator[]×2 + memset×2。ORIG 0x085c5cec
// [b28d-solo-CPrivateStoreMgr] P167 差集归零（op[] 一次后缓存到 -0x20 槽；
// memset→初始化式）。残 85 行 = ORIG 内联 memset 慢路展开（title@-0x14d 非 4
// 对齐 → 「对齐剥离+rep stos+尾字节」vs 4.4 简单 rep stos；P312 pad 实测无效）
// + 槽基差 3（N95）+ N4/P30 寻址。语义已对齐（无缺调用）。
bool CPrivateStore::RecallPrivateStoreItemToMail()
{
    Inven_Item items[14];
    int count = 0;
    for (unsigned int i = 0; i < 0xe; ++i)
    {
        // [b28d-solo-CPrivateStoreMgr] ORIG 0x085c5d2f：operator[] 一次后把
        // 返回引用缓存到 -0x20 槽（`mov %eax,-0x20(%ebp)`），GetState/+2 读/
        // setCopy 实参全用该缓存 ⇒ 旧逐次下标形 P167 EXTRA op[]×2。
        PrivateStoreItem& storeItem = m_items[i];
        // [b28d-PS] P223 双出口岛：ORIG 0x085c5d52..0x085c5d8a 为
        // `sete/test/je L_loopend`（c1 假→循环尾）+ `test/je L_nop`（c2 假→
        // body 后 nop 岛）+ body 尾 `jmp L_loopend` ⇒ 嵌套 if 提供 sete 物化，
        // 内层 `==0 goto` + body 尾 goto 复现两个不同落点。
        if (storeItem.GetState() == 3)
        {
            if (storeItem.m_item.m_addInfo == 0)
                goto next_item;
            items[count++].setCopy(storeItem.m_item);
            goto next_loop;
        }
    next_item:;
    next_loop:;
    }
    // [b28d-solo] ORIG 0x085c5d9x `cmp $0,-0x10; sete al; test al,al; je`
    // （count<=0 跳过）⇒ 4.4 的 `0 < count` 产三目岛，改正写 `count > 0`。
    if (count > 0)
    {
        // [b28d-solo-CPrivateStoreMgr] P214③/P253：ORIG 0x085c5da3/0x085c5e04 为
        // 局部数组初始化内联清零（对齐剥离+rep stos），显式 memset 生 call
        // ⇒ P167 EXTRA memset×2。改初始化式。
        char sender[0x15] = {0};
        // [b28d-PS] 删历史 pad[1]：ORIG 0x085c5e04 title@-0x14d（奇）紧贴
        // sender@-0x4d、items@-0x4a3 紧贴 title ⇒ 内联清零走「and1+and2+rep
        // stos」慢路；pad 使 title 落 -0x14e（偶）丢掉 and1 段（gap 6）。
        char title[0x100] = {0};
        std::strcpy(sender, g_scriptStringManager_.findString(4, "game_server_msg_183", 0));
        std::strcpy(title, g_scriptStringManager_.findString(4, "game_server_msg_186", 0));
        int ret = WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
            sender, items, (unsigned int)count, 0u,
            (unsigned int)m_pSeller->getCurCharacNo(), title,
            (int)std::strlen(title), 0u, m_pSeller->GetServerGroup(), false);
        if (ret != 0)
        {
            // [b28d-solo] P221：ORIG trace 槽 -0x38（深于 items/sender 区），
            // 具名对象令 4.4 排到 -0x50 ⇒ 全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x3c1, 5)(
                "CPrivateStore::RecallPrivateStoreItemToMail Error - Mail Send Fail "
                "User(%d), Charac_no(%d), item_count(%d)",
                m_pSeller->get_acc_id(), m_pSeller->getCurCharacNo(), count);
            return 1;
        }
    }
    ResetItems();
    return 1;
}