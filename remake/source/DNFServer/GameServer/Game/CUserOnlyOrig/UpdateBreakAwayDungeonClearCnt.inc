// ---------------------------------------------------------------------------
// 25. CUser::UpdateBreakAwayDungeonClearCnt @0x086522ea
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x086522ea：调用点/形态已对齐（本波修法见函数内注释）。
void CUser::UpdateBreakAwayDungeonClearCnt()
{
    setBreakAwayDungeonClear(false);

    // ORIG 0x08652322：Acquire 结果免槽直落 ctor 实参（技巧37，同 _saveProper 案）；
    // 具名 stream 槽 -0xc 使 guard 落 -0x14（ORIG -0x10）全域漂移。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1c01), true);
    guard.operator*()->operator<<(0x145);
    // [b28d-solo-CUserOnlyOrig P167 重载选错真 bug] ORIG 0x0865238f 段命中
    // `_ZN12CStreamGuardlsEi`（int 版）而非 lsEj；GetUID() 本仓返回 unsigned int
    // （CUser.h 冻结，不改签名）⇒ 调用点 static_cast<int> 收窄（零指令成本）。
    guard.operator*()->operator<<(static_cast<int>(GetUID()));
    guard.operator*()->operator<<(get_acc_id());
    guard.operator*()->operator<<(getBreakAwayDungeonClearCnt());
    GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
}