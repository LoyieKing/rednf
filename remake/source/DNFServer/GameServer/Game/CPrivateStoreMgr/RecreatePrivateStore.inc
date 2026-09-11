// ---- RecreatePrivateStore（ORIG T 0x85c5b60，报告 RecreatePrivateStore.md）----
// LoadPrivateStoreResponse 通知链经 ISeller 虚表槽 +0x14 调用。前置校验
// （m_pParent 非空 / 状态为 0 / 卖家非空）后：重算开店起点、清围观列表、
// ResetItems 后从 SIG 恢复 14 条目；拍卖代理在线时广播 OPEN_PRIVATE_STORE_GA
// (category 0 / packetID 0xb / size 0xaa)，最后置状态 2 返回 1。
// [b28d-24] DIFF 残29: P167 EXTRA __cxa_guard_abort+_Unwind_Resume（静态局部 guard 段差异）。ORIG 0x085c5b60
int CPrivateStore::RecreatePrivateStore(SIG_PRIVATE_STORE* sig)
{
    if (m_pParent == 0)
        return 0;
    if (m_state != 0)
        return 0;
    if (m_pSeller == 0)
        return 0;

    // ORIG：函数内 static（__cxa_guard 守护的 characterControllerPrivate 单例）。
    static CCharacterControllerPrivate characterControllerPrivate;
    characterControllerPrivate.GetStartPoint(m_pSeller, m_pos);

    m_users.clear();
    ResetItems();
    CopyItems(reinterpret_cast<PrivateStoreItem*>(sig->m_items), 0xe);

    if (GlobalData::s_auction_proxy->IsRunning())
    {
        PCK_AUCTION_OPEN_PRIVATE_STORE_GA packet;
        packet.m_id = (unsigned int)m_pSeller->get_acc_id();
        packet.character_num = m_pSeller->get_charac_count();
        for (int i = 0; i < packet.character_num; ++i)
            packet.character_ids[i] = m_pSeller->get_charac_no(i);
        GlobalData::s_auction_proxy->SendPacket(&packet);
    }

    m_state = 2;
    return 1;
}