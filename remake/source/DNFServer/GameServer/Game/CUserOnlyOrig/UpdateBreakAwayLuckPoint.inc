void CUser::UpdateBreakAwayLuckPoint()
{
    setUsedBreakAwayLuckPoint(false);

    // ORIG 0x086521ec：Acquire 结果 %eax 跨步直落 guard ctor 实参（无 stream 槽，
    // guard 独享 -0x10；具名 stream 反产 spill+槽号全体偏移）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1bf2), true);
    guard.operator*()->operator<<(0x149);
    // [b28d-solo-CUserOnlyOrig P167] 同上：ORIG 命中 lsEi（int 版）。
    guard.operator*()->operator<<(static_cast<int>(GetUID()));
    guard.operator*()->operator<<(get_acc_id());
    GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
