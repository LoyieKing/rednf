// ORIG 0x0842b790 sz 0xdb（_ZN22DB_SecurityCardDisuse11makeRequestEj，
// d_aa_body.cpp objdump 转写）：Acquire("DBThread.cpp",0x6e2d)
//   → CStreamGuard(stream,true) → <<0xfe(int) → <<-1(int)
//   → GetInBuffer<SIG_SECURITY_CARD_DISUSE> → sig+0x00=accId
//   → put(DB_Q(2))。
void DB_SecurityCardDisuse::makeRequest(unsigned int accId)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6e2d), true);
    guard.operator*()->operator<<((int)0xfe);
    guard.operator*()->operator<<((int)-1);
    SIG_SECURITY_CARD_DISUSE* sig =
        guard->GetInBuffer<SIG_SECURITY_CARD_DISUSE>();
    sig->m_mId = accId;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}