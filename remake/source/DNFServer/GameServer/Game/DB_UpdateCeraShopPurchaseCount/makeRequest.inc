// ============================================================================
// DB_UpdateCeraShopPurchaseCount::makeRequest @ 0x0844b31c
// 逐指令转写：Acquire("DBThread.cpp",0xbbd6) → CStreamGuard(s,true) →
//   *guard<<0x2d9 → *guard<<param4 → guard->GetInBuffer<SIG>()（null 则
//   trace(0xbbde) "ptrData is null" 返）→ sig+0xc=count；+0=accId；+4(8B)=itemUid；
//   +8(2B)=0 → MsgQueueMgr::put(DB_Q=2, guard)。
// ============================================================================
// b28d-solo-DB_UpdateCeraShopPurchaseCount: AE（NEAR→AE，ndiff 9→0）—
//   修法：if 块内 cMyTrace 具名对象改全表达式临时链
//   `cMyTrace(f,l,v)("...")`（P221/P37）⇒ trace 槽 -0x1c（ORIG）对齐，
//   guard 保持 -0x24；槽号与 ORIG 逐条一致。82=82 insns，p167 CLEAR(14=14)，
//   compare_tu clean=ae（残差仅 .o 重定位数据地址/call 目标，P264 口径）。
//   ORIG 0x0844b31c。
void DB_UpdateCeraShopPurchaseCount::makeRequest(memberIdentificationNumber_t accId,
                                                 itemIndexOnlyServer_t itemUid,
                                                 int count, int param4)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbbd6), true);
    guard.operator*()->operator<<((int)0x2d9);
    guard.operator*()->operator<<(param4);
    SIG_CERASHOP_PURCHASE_COUNT* sig =
        guard->GetInBuffer<SIG_CERASHOP_PURCHASE_COUNT>();
    if (sig == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbbde, 5)(
            "[PURCAHSE BONUS] ptrData is null");
        return;  // ORIG jmp → ~CStreamGuard 后 ret（无入队）
    }

    sig->m_count = count;
    sig->m_id = accId;
    sig->m_itemUid = (unsigned int)itemUid;   // ORIG +0x4 仅 4 字节写
    sig->m_purchaseCnt = 0;

    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}