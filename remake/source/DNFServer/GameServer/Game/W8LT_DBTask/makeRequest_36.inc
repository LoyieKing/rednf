// ============================================================================
// DB_SecuServiceReward::makeRequest —— ORIG 0x08437844（报告 makeRequest.md 全文）
// ============================================================================
// ORIG 0x8437844：Acquire("DBThread.cpp",0x847c) → CStreamGuard(stream,true)
//   → <<0x19d(int) → <<-1(int) → sig=GetInBuffer<SIG_SECU_SERVICE_REWARD>()
//   → sig+0(uint)=accId、sig+4(char)=flag → put(DB_Q(2))。
// ============================================================================
void DB_SecuServiceReward::makeRequest(unsigned int accId, char flag)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x847c), true);
    guard.operator*()->operator<<((int)0x19d);
    guard.operator*()->operator<<((int)-1);
    SIG_SECU_SERVICE_REWARD* sig =
        guard->GetInBuffer<SIG_SECU_SERVICE_REWARD>();
    sig->m_mId = accId;
    sig->m_flag = flag;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}