// ---- CreatePrivateStore（ORIG T 0x85c53ea，报告 private_store__CPrivateStore/CreatePrivateStore.md）----
// ISeller 虚表槽 +0x00。开店主流程：权限/区域/频道校验，可选 doll 物品登记，
// 构造碰撞位并注册村庄对象，置状态 1，命令包与广播包，有队伍则离队分发，
// 历史/拍卖代理上报。
// [b28d-24] DIFF 残69: P167 EXTRA getCurCharacName×1；余帧/槽分配（480=480）。ORIG 0x085c53ea
// [b28d-solo-CPrivateStoreMgr] P167 归零（删孤立 getCurCharacName 调用）。残 234
// 行 = 块序（ORIG 在 IsPermissionPrivateStore 之后插 G_GameWorld/IsPVPChannel/
// Do(0x3e) 段）+ 条件极性（ORIG xor$1 形，P292）+ 帧/槽（N82）。指令数 476 vs 480。
// [L14] 本轮复核：480/480，2 差异块 = ①ORIG `mov 0x8(%ebp),%edx; add $0x14,%edx`
// vs 我方 `mov this,%eax; lea 0x14(%eax),%edx`（add-vs-lea，N4 族）；
// ②`lea -0x10a(%ebp)` vs `-0x10e`（槽差 4，N82）⇒ 封顶。
// [K2 2026-09-11 复验] fresh .o：24 组 slot-pair = +4×16 / +28×3 / -4×2（多段
// 常数 ⇒ P331 判不可修）；帧 sub 0x15c 双侧相同；对象级错位（auctionPacket
// character_ids `mov %eax,-0x100(%ebp,%edx,4)` ↔ `-0x104`、newPos/regPos 组
// -0x60↔-0x64 / -0x54↔-0x50）⇒ 维持 N82/N4 封顶。
bool CPrivateStore::CreatePrivateStore(unsigned char zoneA, unsigned char zoneB,
                                       short posX, short posY, unsigned short dollSlot)
{
    m_errorHandler.Set(m_pSeller, 1, 0x58);

    if (m_pSeller == 0)
        return false;
    if (m_pSeller->IsBusyDisjointStore())
    {
        m_errorHandler.Do(0x3e);
        return false;
    }
    if ((signed char)m_pSeller->getCurCharacVill() == 7)
    {
        m_errorHandler.Do(0x3e);
        return false;
    }
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }

    // 权限与 doll 槽互斥：有私商权限必须传 0xffff；无权限禁止传 0xffff。
    // [b28d-solo] ORIG 0x085c5512 `xor eax,0x1; test al,al; je 0x159`（取反 +
    // 两臂互换的块序）⇒ 写否定式 + 交换两臂（P222/P59）。
    if (!m_pSeller->IsPermissionPrivateStore())
    {
        if (dollSlot == 0xffff)
        {
            m_errorHandler.Do(0x3f);
            return false;
        }
    }
    else if (dollSlot != 0xffff)
    {
        m_errorHandler.Do(0x3f);
        return false;
    }

    if (m_pSeller->get_state() == 6)
    {
        m_errorHandler.Do(0x3e);
        return false;
    }
    if (G_GameWorld()->GetChannelType() == 2)
    {
        m_errorHandler.Do(0x3e);
        return false;
    }
    if (m_state != 0)
    {
        m_errorHandler.Do(0x14);
        return false;
    }

    int restrictResult =
        G_GameWorld()->check_restrictive_commercial_transaction_zone(
            (int)zoneA, (int)zoneB, (int)posX, (int)posY);
    if (restrictResult != 0)
    {
        m_errorHandler.Do(restrictResult);
        return false;
    }
    if (G_GameWorld()->IsPVPChannel())
    {
        m_errorHandler.Do(0x3e);
        return false;
    }
    if (!IsAreaNearEntranceDungeon(m_pSeller))
    {
        m_errorHandler.Do(0x3e);
        return false;
    }

    if (dollSlot != 0xffff)
    {
        int dollError = CheckPrivateStoreDoll(dollSlot);
        // [BigStore P225] ORIG 0x085c5708 `cmpl $0,-0x20; setne %al; test; je`
        // 物化形 ⇒ while 复现（4.4 的 `!=` 直跳无 setne）。
        while (dollError != 0)
        {
            m_errorHandler.Do(dollError);
            return false;
        }
        SetDollCreated(true);
        // [BigStore P329] ORIG 0x085c5755 getCurCharacInvenR 结果 eax 于 0x085c576c
        // 直落 GetInvenSlot esp+0x04 实参槽，无 inventory 具名缓存槽 ⇒ 链式内联。
        Inven_Item dollItem =
            m_pSeller->getCurCharacInvenR()->GetInvenSlot(1, (int)dollSlot);
        SetDollItemID((unsigned long)dollItem.m_addInfo);  // ORIG 读隐藏返回副本 +2（物品索引）
    }

    if (!isDollCreated())
        m_pSeller->set_position(posX, posY, 0);

    CPrivateStorePos newPos(this, (int)posX, (int)posY);
    m_storePos = newPos;

    // [b28d-solo] 同 MoveToGate/RemovePrivateStore：ORIG 0x085c58xx 在 Zone ctor
    // 前 `lea ebx,[eax+0x4c]` 预取 &m_storePos（匿名 sret 临时；具名 zone 令
    // 4.4 排到 packet 之下并丢失 ebx 保活）。
    if (!GlobalData::s_villageObjectMgr->register_object(
            village_object::Zone(zoneA, zoneB), &m_storePos))
    {
        m_errorHandler.Do(0x38);
        return false;
    }

    // ORIG 0x085c5879-0x085c58bd：4 参 ctor（W 0x85cb72c 调用发射）构造临时，
    // 再 6 字节拷入 m_pos(+0x24 int 段/+0x28 short 段)——恢复原构造步形并复原
    // 历轮因定义未使用而丢失的 W 符号。
    PrivateStorePos regPos(zoneA, zoneB, posX, posY);
    m_pos = regPos;

    m_users.clear();
    ResetItems();
    m_state = 1;

    CPacketBufPrivate packet;
    packet.MakeCmdpacketCreatePrivateStore();
    m_pSeller->Send(packet);

    packet.MakeNotipacketCreatePrivateStore((unsigned short)GetSellerIndex(), m_pos,
                                            isDollCreated() ? true : false);
    // [b28d-PS] ORIG 0x085c5945..0x085c5967：实参从 m_pos(+0x24/+0x25) 两次
    // movzbl 读取（`movzbl 0x25(%eax),%eax; movzbl %al,%edx`），非参数槽
    // zoneA/zoneB ⇒ 源码读 m_pos.m_a/m_b。
    CBroadcastPrivate::Send((int)m_pos.m_a, (int)m_pos.m_b, packet);

    // 有队伍时经函数内静态 DisPatcher_LeaveParty 分发离队（ORIG leave_party 局部静态）。
    if (m_pSeller->GetParty() != 0)
    {
        static DisPatcher_LeaveParty leave_party;  // ORIG 函数内静态（guard 0x09472e28/本体 0x09472e40）
        // ORIG call 0x0822b766 = InterfacePacketBuf::get()（PacketBuf* 无参 getter）
        leave_party.dispatch_template(m_pSeller, *packet.get());
    }

    {
        // [b28d-solo] P227：ORIG 在 ctor 前求 get_unique_id 并跨 ctor 保活。
        cMyTrace(__PRETTY_FUNCTION__, 0x335, 0)(
            "CreatePrivateStore() uid:%d\n",
            (unsigned int)m_pSeller->get_unique_id() & 0xffff);
    }
    // [b28d-solo-CPrivateStoreMgr] P167 EXTRA getCurCharacName×1：删孤立调用，
    // 仅保留 historyLog 实参位的一次（ORIG 调用点唯一）。
    m_pSeller->m_historyLog.CreatePrivateStore(          // CUser 内嵌 +0x79700（CUser.h:1596）
        (int)zoneA, (int)zoneB, m_pSeller->getCurCharacName());

    if (GlobalData::s_auction_proxy->IsRunning())
    {
        PCK_AUCTION_OPEN_PRIVATE_STORE_GA auctionPacket;
        auctionPacket.m_id = (unsigned int)m_pSeller->get_acc_id();
        auctionPacket.character_num = m_pSeller->get_charac_count();
        for (int i = 0; i < auctionPacket.character_num; ++i)
            auctionPacket.character_ids[i] = m_pSeller->get_charac_no(i);
        GlobalData::s_auction_proxy->SendPacket(&auctionPacket);
    }
    return true;
}