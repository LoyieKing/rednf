// ORIG 0x0842bce8 sz 0xf1（_ZN33DB_SecurityCardUpdateValidityTime11makeRequestEjj）：
// accId==0 直接返回；Acquire("DBThread.cpp",0x6ebb) → CStreamGuard(stream,true)
//   → <<0x100(int) → <<-1(int)
//   → sig=GetInBuffer<SIG_SECURITY_CARD_UPDATE_VALIDITYTIME>()
//   → sig+0x00(uint)=accId、sig+0x04(uint)=flag → put(DB_Q(2))。
void DB_SecurityCardUpdateValidityTime::makeRequest(unsigned int accId,
                                                    unsigned int flag)
{
    if (accId == 0)
        return;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6ebb), true);
    guard.operator*()->operator<<((int)0x100);
    guard.operator*()->operator<<((int)-1);
    SIG_SECURITY_CARD_UPDATE_VALIDITYTIME* sig =
        guard->GetInBuffer<SIG_SECURITY_CARD_UPDATE_VALIDITYTIME>();
    sig->m_mId = accId;
    sig->m_flag = flag;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}