// ============================================================================
// b21 NEAR：逐指令语义与 ORIG 0x0844b0cc 全等；残余差异仅
//   ① m_cnt 自增 N-86b 同算术多 codegen（lea $0x1 vs add $0x1，结果全同）
//   ② cMyTrace/sig 局部栈槽偏移（lea -0x28 vs -0x40 帧布局产品）
//   ③ 数据地址/call 目标【链接期解析，AE 归一】。语义完成，byte 层 N 封顶。
//   【b28d-solo 复核】① 已对齐（lea 0x1(%eax),%edx 逐条同形）；② 已由
//   cMyTrace 临时链翻正（见下方 b28d-solo 判词）；③ 仅余 AE 归一项。
// DB_UpdateCeraShopPurchaseCount::dispatch @ 0x0844b0cc
// 逐指令转写：GetOutBuffer<SIG_CERASHOP_PURCHASE_COUNT> → null 则 trace(0xbbac)
//   "ptrData is null." 返 false；_selectPurchaseCount(m_id, &purchase_cnt) 失败
//   trace(0xbbb2) "select query fail." 返 false；purchase_cnt==0 →
//   cnt=1 + _insertPurchaseCount(m_id)，否则 cnt+1 + _updatePurchaseCount(m_id,cnt)；
//   共用出口：Acquire("DBThread.cpp",0xbbc1) → CStreamGuard(s,true) → *guard<<0x2d9
//   → *guard<<slot → guard->GetInBuffer<SIG>()（null 则 trace(0xbbc9) 返 false）
//   → 16B 逐字拷贝 → MsgQueueMgr::put(DISPATCH_Q=1, guard) → 返 true。
// ============================================================================
// b28d-solo-DB_UpdateCeraShopPurchaseCount: AE（NEAR→AE，ndiff 13→0）—
//   修法：3 个 cMyTrace 具名对象改全表达式临时链
//   `cMyTrace(f,l,v)("...")`（P221/P37）⇒ 4.4 互斥/嵌套块对象槽序翻正为
//   ORIG 深→浅（trace -0x40/-0x30/-0x20、guard -0x48），槽号逐条一致。
//   154=154 insns，p167 CLEAR(22=22)，compare_tu clean=ae（残差仅 .o 重定位
//   数据地址/call 目标，P264 口径）。ORIG 0x0844b0cc。
bool DB_UpdateCeraShopPurchaseCount::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_CERASHOP_PURCHASE_COUNT* sig =
        stream->GetOutBuffer<SIG_CERASHOP_PURCHASE_COUNT>();
    if (sig == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbbac, 5)(
            "[PURCAHSE BONUS] ptrData is null.");
        return false;
    }

    if (!_selectPurchaseCount(sig->m_id, sig->m_purchaseCnt))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbbb2, 5)(
            "[PURCAHSE BONUS] select query fail.");
        return false;
    }

    if (sig->m_purchaseCnt == 0)
    {
        sig->m_purchaseCnt = 1;
        _insertPurchaseCount(sig->m_id);
    }
    else
    {
        sig->m_purchaseCnt = sig->m_purchaseCnt + 1;
        _updatePurchaseCount(sig->m_id, sig->m_purchaseCnt);
    }

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbbc1), true);
    guard.operator*()->operator<<((int)0x2d9);   // ORIG operator*() → operator<<(int)
    guard.operator*()->operator<<(slot);         // ORIG 0x10(%ebp) 第 3 形参
    SIG_CERASHOP_PURCHASE_COUNT* replySig =
        guard->GetInBuffer<SIG_CERASHOP_PURCHASE_COUNT>();
    if (replySig == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbbc9, 5)(
            "[PURCAHSE BONUS] ptrData is null");
        return false;
    }

    *replySig = *sig;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return true;
}