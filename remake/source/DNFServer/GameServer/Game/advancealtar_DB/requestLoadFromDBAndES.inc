bool DBAndExchangeServerProc::requestLoadFromDBAndES(CUser& user, MSG_CHARAC_SELECT& msg)
{
    user.m_advanceAltar.reset();  // ORIG CharacAdvanceAltarManager::reset @0x08131660

    // [b28d-solo-advancealtar_DB] P89/P259：ORIG 无 stream 具名槽（0x0813379e
    //   `mov &s_stream_pool,%eax` → Acquire 结果经 eax 直压 ctor 的 0x4(%esp)，
    //   guard 槽 -0x14）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localglobal/global_AdvanceAltar.cpp", 0xc3a), true);
    guard.operator*()->operator<<(0x19);
    guard.operator*()->operator<<(static_cast<int>(user.GetUID()));
    user.DisableCharacInfo(CACHE_ADVANCE_ALTAR);  // ORIG push 0xa（枚举名 Prod_DispatcherHelpers 落地）

    if (user.IsValidLastLoginCharac())
    {
        guard.operator*()->operator<<(user.GetLastLoginChannelNo());
        guard.operator*()->operator<<(user.GetLastLoginCharacNo());
    }

    _CharacAdvanceAltarDbData* data =
        guard.operator->()->GetInBuffer<_CharacAdvanceAltarDbData>();
    std::memset(data, 0, 0x51c);
    // ORIG 0x081338d6 `call get_charac_no; mov -0xc(%ebp),%edx; mov %eax,(%edx)`
    //   = LHS-first 直存（P211 同型）⇒ 去掉旧 static_cast<unsigned int>（异型化反产
    //   eax→edx 中转）。
    data->m_characNo = user.get_charac_no(static_cast<int>(msg.m_slot));

    // ORIG 0x081338ee `test %al,%al; je <put(2)>` ⇒ 正极性：IsValid 真走 fall-through
    //   put(队列5)，假走 DB_Q(2)。
    if (user.IsValidLastLoginCharac())
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(5), guard);  // 第 6 槽（ORIG 无枚举名）
    else
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
