// ORIG 0x816bf6a：IsEventing(0) 与 IsFirstReset() 双 early-return 后
// Acquire → CStreamGuard(stream,true) → internal_stream(7,-1) →
// operator-> 后 GetInBuffer 判空 → put(DB_Q)。无任何空指针检查。
void CEventStayTime::dailyresetData()
{
    if (!IsEventing(0))
        return;
    if (IsFirstReset())
        return;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Event/EventStayTimeCharacter.cpp", 0x4a), true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(7), -1);
    Taiwan::SigResetStayTimeEvent* sig =
        guard->GetInBuffer<Taiwan::SigResetStayTimeEvent>();
    if (sig != 0)
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}