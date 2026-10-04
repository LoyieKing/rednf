// ----------------------------------------------------------------------------
// ORIG 0x086501c8 CUser::SaveInventoryOnly() uint
// [b28d-22 N82] ORIG 0x086501c8：p167=CLEAR，gap=1；语义完成、byte 层帧槽/物化族封顶。
// [A06 2026-09-10 归零] 192/192 AE/identical（delta=0）。修法 = P259：删 `Stream* s`
// 具名局部，Acquire 结果直落 CStreamGuard ctor 实参（ORIG 0x086501c8 段
// `call Acquire; mov %eax,0x4(%esp)`，无 stream 槽；旧形多一对 store/load 并顶浅全帧）。
// 此项原判 N82 已作废。
// [K7 2026-09-11 §6.5 PRETTY 串盲区] ORIG .rodata 实串 = `bool CUser::SaveInventoryOnly()`
// ⇒ 返回类型 bool（同 CUser.h:3736）。
bool CUser::SaveInventoryOnly()
{
    if (isLocked4DataLoad()) {
        // [L6 判据⑨重核 · P37/P227] ORIG 0x086501c8 段：先求 varargs（`call NumberToString;
        // mov %eax,%esi` + `call getCurCharacNo; mov %eax,%ebx`）**再** `cMyTraceC1`（常量
        // 5/0x187a/串三实参）**最后** operator() —— 两语句形把 C1 提到 varargs 求值之前
        // （构造即调用形），故用全表达式临时链保持 ORIG 求值序。
        cMyTrace((char*)__PRETTY_FUNCTION__, 0x187a, 5)(
            "Can not save character until loading done(m_id: %s, charac_no: %d",
            NumberToString(get_acc_id(), 0), getCurCharacNo());
        return 0;
    }
    if (G_GameWorld()->GetChannelType() == 7) return 0;

    // [A06 2026-09-10 P259] ORIG 0x086501c8 段 `call Acquire; mov %eax,0x4(%esp)`（结果
    // 直落 guard ctor 实参、无 stream 具名槽）；旧形多一对 `mov %eax,-0x10; mov -0x10,%eax`
    // 并顶浅后续全部槽（gap 2）。
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x1888), true);
    guard.operator*()->operator<<(0x23);           // ORIG 0x086501c8：deEv→lsEi 链
    guard.operator*()->operator<<((int)GetUID());  // ORIG 同型
    SIG_SAVE_INVENTORY* inv = (SIG_SAVE_INVENTORY*)
        guard.operator->()->GetInBuffer<SIG_SAVE_INVENTORY>();  // ORIG ptEv 链

    _GetSaveInvenBase(inv);
    inv->m_saveFlag = 2; /*ORIG sig+0x08 save type*/                              // +0x08 save type

    memcpy(inv->m_invenType0,  getCurCharacInvenR()->GetInvenStart(0), 0x53e);
    memcpy(inv->m_invenType1,    getCurCharacInvenR()->GetInvenStart(1), 0x4a58);
    inv->m_cargoCapacity = (unsigned int)getCurCharacCargoR()->GetCapacity();
    memcpy(inv->m_cargo,
           getCurCharacCargoR()->get_cargo_start(), 0x2438);
    memcpy(inv->m_invenType3 /*invenRewardZone*/,
           getCurCharacInvenR()->GetInvenStart(3), 0x39aa);
    memcpy(inv->m_creatureTail,
           &getCurCharacInvenR()->m_invenItem[22], 0xf4);  // +0x55a = m_invenItem[22]

    m_characExpand.update(this, (ENUM_CHARAC_EXPAND_TYPE)9);
    removeRentalItemInfoFromInvenBuf(inv, 1);
    removeRentalItemInfoFromInvenBuf(inv, 2);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 1;
}