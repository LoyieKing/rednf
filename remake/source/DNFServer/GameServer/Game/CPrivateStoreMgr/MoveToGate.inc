// ---- MoveToGate（ORIG T 0x85c751a，报告 MoveToGate.md）----
// ISeller 虚表槽 +0x0c：收店回城门。已登记位置时先广播拆除通知并注销村庄对象，
// 再把卖家移回开店起点；随后发送 MoveToGate 命令包。
// [b28d-24] DIFF 残26: P167 归零；残差=帧/槽分配（P112/N82）。ORIG 0x085c751a
// [b28d-solo-CPrivateStoreMgr] 已修：packet 声明下移至双守卫后（ORIG 0x085c754a
// 才 C1）；unregister_object 返回类型 int→bool（P292 档①，调用点 xor$1 形）。
// 残 24 行 = ①块外 packet(-0x18)/块内 zone(-0xa) 槽方向镜像 vs 我们 -0x14/-0x16
// ⇒ N93；②ORIG 在 Send 后把 &m_storePos 预取 %ebx 跨 Zone ctor 保活、4.4 在
// 调用点才 lea（P52 实参物化时机，具名局部会引入槽）⇒ 跨版本寄存器分配。
bool CPrivateStore::MoveToGate()
{
    m_errorHandler.Set(m_pSeller, 1, 0x59);

    if (m_pSeller == 0)
        return false;
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }

    // ORIG 0x085c754a 才构造 packet（前两个守卫失败路径无 C1/D1）⇒ 声明下移。
    CPacketBufPrivate packet;

    if (m_pos.m_a != 0 || m_pos.m_b != 0 ||
        m_pos.m_field2 != 0 || m_pos.m_field4 != 0)
    {
        packet.MakeNotipacketRemovePrivateStore((unsigned short)GetSellerIndex());
        CBroadcastPrivate::Send((int)m_pos.m_a, (int)m_pos.m_b, packet);

        // [b28d-solo] ORIG 0x85c7609：&m_storePos 先入 %ebx（跨 Zone ctor 保活），
        // Zone 以匿名 sret 临时落 -0xa；具名 zone 令 4.4 把块内对象排到 packet
        // 之下（-0x16）⇒ 改匿名临时实参（P52/P134）。
        if (!GlobalData::s_villageObjectMgr->unregister_object(
                village_object::Zone(m_pos.m_a, m_pos.m_b), &m_storePos))
        {
            LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x658,
                                  "unregister_object error");
        }

        g_CharacterController.MoveCharacterStartPoint(m_pSeller);
    }

    packet.MakeCmdpacketMoveToGate();
    m_pSeller->Send(packet);
    return true;
}