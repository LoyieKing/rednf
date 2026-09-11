// ---- RemovePrivateStore（ORIG T 0x85c5fd0，报告 RemovePrivateStore.md）----
// ISeller 虚表槽 +0x04。拆店主流程：按状态退还条目（doll 走邮件，非 doll 走
// RecallPrivateStoreItem），围观广播强拆包，历史/拍卖上报，注销村庄对象，
// 释放卖家表项并 Clear。sendFail=false 且条目退还失败时中止。
// [b28d-solo-CPrivateStoreMgr] 已修真语义 bug：sendFail 守卫反转（ORIG
// 0x085c616c xor$1 ⇒ `if(!sendFail) return false;`）；trace 改 P227 全表达式
// 临时链（get_unique_id 跨 ctor %ebx 保活）。残 46 行 = 块级对象槽方向镜像
// （ORIG packet -0x28/zone -0x1a/trace -0x18 vs 我们 -0x24/-0x26/-0x3c）⇒ N93
// + ORIG Send 后预取 &m_storePos 入 %ebx（P52 物化时机）⇒ N 封顶。
bool CPrivateStore::RemovePrivateStore(bool sendFail)
{
    // ORIG(0x085c60xx)：判空先于 Set，失败返路径不经过 Set；仅通过后才 Set。
    // 两真语义：sendFail 为 false 返回 false（非 true）；锁列表用 R 版（const 链）。
    m_errorHandler.Set(m_pSeller, 1, 0x59);
    if (m_pSeller == 0)
        return false;
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }
    if (m_state == 0)
    {
        m_errorHandler.Do(0x3b);
        return false;
    }

    CPacketBufPrivate packet;
    if (m_state == 2)
    {
        if (isDollCreated())
        {
            if (RecallPrivateStoreItemToMail())
                SavePrivateStore();
        }
        else
        {
            if (RecallPrivateStoreItem())
            {
                SavePrivateStore();
                m_pSeller->UpdateData();
                m_pSeller->send_itemspace(0);
                m_pSeller->send_itemspace(1);
                m_pSeller->getCurCharacInvenR()->SendItemLockListInven();
                m_pSeller->getCurCharacInvenR()->SendItemLockListAvatar();
            }
            else
            {
                SavePrivateStore();
                m_pSeller->UpdateData();
                m_errorHandler.Do(0x16);
                // 真语义 bug 修复：ORIG 0x085c616c `movzbl sendFail; xor $1;
                // test %al,%al; je 0x1b1` ⇒ 条件为 !sendFail（sendFail==1 时
                // 继续，==0 时 return false）；旧重建写成 `if (sendFail)` 反了。
                if (!sendFail)
                    return false;
            }
        }

        if ((int)m_users.size() != 0)
        {
            packet.MakeNotipacketForceRemovePrivateStore();
            SendUsers(&packet, 0, 0);
        }
    }

    // ORIG P167：getCurCharacName 仅 1 次（原先多一条返回值丢弃的死调用）。
    m_pSeller->m_historyLog.RemovePrivateStore(
        (int)m_pos.m_a, (int)m_pos.m_b, m_pSeller->getCurCharacName());

    if (GlobalData::s_auction_proxy->IsRunning())
    {
        PCK_AUCTION_CLOSE_PRIVATE_STORE_GA auctionPacket;
        auctionPacket.m_id = (unsigned int)m_pSeller->get_acc_id();
        GlobalData::s_auction_proxy->SendPacket(&auctionPacket.m_header);
    }

    packet.MakeCmdpacketRemovePrivateStore();
    m_pSeller->Send(packet);

    packet.MakeNotipacketRemovePrivateStore((unsigned short)GetSellerIndex());
    CBroadcastPrivate::Send((int)m_pos.m_a, (int)m_pos.m_b, packet);

    // [b28d-solo] 同 MoveToGate：匿名 Zone 临时 + &m_storePos 实参（ORIG
    // 0x85c62xx `lea ebx,[eax+0x4c]` 先物化跨 Zone ctor 保活；具名 zone 令
    // 4.4 把块内对象排到 packet 之下 -0x26，ORIG 为 -0x1a）。
    if (!GlobalData::s_villageObjectMgr->unregister_object(
            village_object::Zone(m_pos.m_a, m_pos.m_b), &m_storePos))
    {
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x473,
                              "unregister_object error");
    }

    m_pParent->FreeStoreSeller(m_pSeller);
    // P227 全表达式临时链：ORIG 在 ctor 前把 get_unique_id 读入 %ebx 跨 ctor
    // 保活（0x085c62xx 段），具名 trace 对象会退化为 ctor 后求值。
    cMyTrace(__PRETTY_FUNCTION__, 0x478, 0)(
        "RemovePrivateStore() uid:%d\n",
        (unsigned int)m_pSeller->get_unique_id() & 0xffff);
    Clear();
    return true;
}