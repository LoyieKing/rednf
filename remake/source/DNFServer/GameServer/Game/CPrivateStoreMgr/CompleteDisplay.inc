// ---- CompleteDisplay（ORIG T 0x85c7800，报告 CompleteDisplay.md）----
// ISeller 虚表槽 +0x08：确认开店告示。MoveItems 成功后保存、刷新物品锁视图，
// doll 存在时消耗对应 doll 槽道具；写入店名，广播 noti 并回发 cmd，置状态 2。
// [b28d-24] DIFF 残12: P167 归零；残差=帧/槽分配（P112/N82）。ORIG 0x085c7800
bool CPrivateStore::CompleteDisplay(char const* subject, unsigned short dollSlot)
{
    m_errorHandler.Set(m_pSeller, 1, 0x5a);

    if (m_pSeller == 0)
        return false;
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }

    if (!MoveItems())
        return false;

    SavePrivateStore();
    m_pSeller->UpdateData();
    m_pSeller->send_itemspace(0);
    m_pSeller->send_itemspace(1);
    m_pSeller->getCurCharacInvenR()->SendItemLockListInven();
    m_pSeller->getCurCharacInvenR()->SendItemLockListAvatar();

    // 无 doll 却指定了 doll 槽道具则拒绝（ORIG：isDollCreated!=0 或 slot==0xffff 才放行）。
    if (!(isDollCreated() != 0) && dollSlot != 0xffff)
        return false;

    if (isDollCreated())
    {
        int useResult = m_pSeller->getCurCharacInvenW()->use_item((int)dollSlot, 1, 1);
        m_pSeller->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, (int)dollSlot);
        if (useResult != 0)
        {
            // P227 全表达式临时链：令 4.4 把 dollSlot 读入 %ebx 跨 ctor 保活
            // （ORIG 0x1a5 movzwl→%ebx、0x1cc mov %ebx,0x8(%esp)）。
            cMyTrace(__PRETTY_FUNCTION__, 0x6c1, 5)(
                "CPrivateStore::CompleteDisplay() ERROR!!! : use_item(%d)", (int)dollSlot);
            return false;
        }
    }

    m_name = subject;

    CPacketBufPrivate packet;
    packet.MakeNotipacketCompleteDisplay((unsigned short)GetSellerIndex(), m_name,
                                         isDollCreated() ? true : false);
    CBroadcastPrivate::Send((int)m_pos.m_a, (int)m_pos.m_b, packet);

    packet.MakeCmdpacketCompleteDisplay(isDollCreated() ? true : false);
    m_pSeller->Send(packet);

    m_state = 2;
    return true;
}