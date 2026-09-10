void CUser::SaveMileage()
{
    if (G_GameWorld()->GetChannelType() == 7) return;

    // ORIG 0x08650636：Acquire 结果 %eax 直落 guard ctor 0x4(%esp)，无 s 槽；
    // guard 槽 -0x14（技巧36 删中转整帧平移）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x18e4), true);
    guard.operator*()->operator<<(0x1fc);          // ORIG 0x086505f6：deEv→lsEi 链
    guard.operator*()->operator<<((int)GetUID());  // ORIG 同型
    // ORIG 0x08650636/0x08650642：Acquire 结果免槽直落 ctor 实参、guard -0x14、
    // sig -0xc（删 s 中转后 4.4 帧对齐 ORIG）；store 段 LHS 地址预载差 = N76
    SIG_MILEAGE_STRUCT* sig = guard.operator->()->GetInBuffer<SIG_MILEAGE_STRUCT>();  // ORIG ptEv 链
    sig->m_accId = get_acc_id();
    sig->m_characNo = getCurCharacNo();
    sig->m_mileage = GetMileage();
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
