CQuickParty* CQuickPartySystemManager::find_quick_party_in_pool(
    std::list<int>& pool)
{
    CQuickParty* qp = 0;             // ORIG -0x10 头部 movl $0x0 初始化
    int qpIndex;                     // ORIG -0xc 深槽
    while (!pool.empty())
    {
        qpIndex = pool.front();
        pool.pop_front();
        qp = G_CGameManager()->GetQuickParty(qpIndex);
        // b28c-lane3：ORIG 0x0826aa43 `jne 0x96`（直跳函数尾 return 块）。
        // 循环内 `return qp` 会被 4.4 物化成 `je <trace>; mov -0x10,%eax; jmp`，
        // 改 break + 循环外 return 后 4.4 与 ORIG 同为 jne 直达尾块。
        if (qp != 0)
            break;
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x29c, 5);
        trace("QUICK_PARTY_LOG : NO INTENDED ROUTINE!!");
    }
    return qp;
