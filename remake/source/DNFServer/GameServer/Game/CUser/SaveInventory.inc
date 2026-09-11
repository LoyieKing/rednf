// 0x0864fe52（ORIG CUser::SaveInventory()）
// ORIG 反编译控制流（docs/class_func_reports/CUser/SaveInventory.md）：
//   1) isLocked4DataLoad() 非零 → cMyTrace 报错 "Can not save character until
//      loading done(m_id: %s, charac_no: %d" 并返回 0；
//   2) G_GameWorld()->GetChannelType()==7 → 返回 0；
//   3) 否则 BigStreamPool::Acquire + CStreamGuard 序列化 0xad99 字节的
//      SIG_SAVE_INVENTORY：header(0x23)+UID → memset 整块 0 → _GetSaveInvenBase 填
//      header 区 → 写 m_saveFlag=0 → GetInvenStart(0/1/3) 三块背包 + inven[22..25]
//      宠物尾 → RemoveKCItem（state==10 时再 RemoveFPItem）→ cargo 容量+数据 →
//      m_characExpand.update(this,9) → removeRentalItemInfoFromInvenBuf(1/2) →
//      MsgQueueMgr::put(DB_Q,guard) → 返回 1。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864fe52 | _ZN5CUser13SaveInventoryEv
bool CUser::SaveInventory()   // b28c-L5 SIGFIX（P174）
{
    if (isLocked4DataLoad())
    {
        // ORIG 0x0864fe69-0x0864fed1：getCurCharacNo→%esi、NumberToString(get_acc_id(),0)→%ebx
        // 均在 cMyTrace ctor **之前**右→左求值并跨 ctor 保活（无 accIdStr 具名槽、无 -0xc 往返）
        // ⇒ P227 全表达式临时链（历史具名 trace+accIdStr 致帧 0x40 与槽漂移）。
        cMyTrace(__PRETTY_FUNCTION__, 0x181a, 5)(
            "Can not save character until loading done(m_id: %s, charac_no: %d",
            NumberToString(get_acc_id(), 0), getCurCharacNo());
        return 0;
    }
    if (G_GameWorld()->GetChannelType() == 7)
    {
        return 0;
    }

    // ORIG 0x0864ff11-0x0864ff2b：Acquire 结果 %eax 直传 C1（无 stream 具名槽，
    // 槽号 -0x24 而非 -0x28）⇒ P0 内联实参，禁 Stream* 缓存。
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x1828), true);
    // ORIG：每条 << 前 call operator*()，GetInBuffer 前 operator->()（P167 缺 3）。
    guard.operator*()->operator<<((int)0x23);          // SIG_SAVE_INVENTORY
    guard.operator*()->operator<<((int)GetUID());

    SIG_SAVE_INVENTORY* sig =
        guard.operator->()->GetInBuffer<SIG_SAVE_INVENTORY>();
    memset(sig, 0, 0xad99);
    _GetSaveInvenBase(sig);
    sig->m_saveFlag = 0;

    // P183：ORIG 每块背包独立 getCurCharacInvenR() 重物化（4 处），禁 inven 缓存。
    memcpy(sig->m_invenType0, getCurCharacInvenR()->GetInvenStart(0), 0x53e);
    memcpy(sig->m_invenType1, getCurCharacInvenR()->GetInvenStart(1), 0x4a58);
    memcpy(sig->m_invenType3, getCurCharacInvenR()->GetInvenStart(3), 0x39aa);
    // CInventory +0x55a = &m_invenItem[22]（ORIG 原始偏移；宠物尾 4 项）
    memcpy(sig->m_creatureTail,
           &getCurCharacInvenR()->m_invenItem[22], 0xf4);

    CInventory::RemoveKCItem(sig->m_invenType0, sig->m_invenType1);
    if (get_state() == 10)
    {
        CInventory::RemoveFPItem(sig->m_invenType0, sig->m_invenType1);
    }

    // P183：ORIG 两次独立 getCurCharacCargoR()（GetCapacity 一次、get_cargo_start
    // 再一次），禁 CCargo* 缓存变元。
    sig->m_cargoCapacity = getCurCharacCargoR()->GetCapacity();
    memcpy(sig->m_cargo, getCurCharacCargoR()->get_cargo_start(),
           ARAD::DATABASE::sourceCargoSize);

    m_characExpand.update(this, (ENUM_CHARAC_EXPAND_TYPE)9);
    removeRentalItemInfoFromInvenBuf(sig, 1);
    removeRentalItemInfoFromInvenBuf(sig, 2);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}