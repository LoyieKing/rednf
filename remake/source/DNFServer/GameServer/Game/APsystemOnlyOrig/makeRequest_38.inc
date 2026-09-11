// ORIG 0x0843b946 sz 0x132（_ZN23DB_SaveAccountCargo11makeRequestEjiP13CAccountCargo）：
// Acquire("DBThread.cpp",35999) → CStreamGuard(stream,true)
//   → <<0x1f1(int) → <<param_1(int) → <<param_2(uint)
//   → sig=GetInBuffer<SIG_ACCOUNT_CARGO_DATA>() → memset 0xd60
//   → sig+0x00(uint)=cargo->m_capacity(+0xd60)、+0x04(uint)=cargo->m_money(+0xd5c)
//   → memcpy(sig+0x08, cargo+0x04, 0xd58) → put(DB_Q(2))
//   → CAccountCargo::SetStable(cargo)。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0843b946
// [b28d-solo-4] §6.2 三件套：p167 调用点差集 CLEAR（ORIG 17 = OUR 17，0 missing/0 extra）
//   + gap −1（88/89）⇒ 语义完成；余 N104 EH 尾块相对序（正常路径 epilogue 就地内联、
//   catch/Unwind 块压最末 vs ORIG 独立尾块）⇒ 按 N104 封顶。
void DB_SaveAccountCargo::makeRequest(int userIdx, unsigned int accId,
                                      CAccountCargo* cargo)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8c9f), true);
    guard.operator*()->operator<<((int)0x1f1);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)accId);
    // [Lane31W8LTDBTaskView] 权威头直取（去 cast；ORIG RHS-first）。
    SIG_ACCOUNT_CARGO_DATA* sig =
        guard->GetInBuffer<SIG_ACCOUNT_CARGO_DATA>();
    memset(sig, 0, 0xd60);
    sig->m_capacity = cargo->m_capacity;
    sig->m_money = cargo->m_money;
    memcpy(sig->m_cargo, &cargo->m_slots[0], 0xd58);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    cargo->SetStable();
}