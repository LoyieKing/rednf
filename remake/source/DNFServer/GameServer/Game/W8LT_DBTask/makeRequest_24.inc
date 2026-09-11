// ORIG 0x0843a34e sz 0x113（_ZN32DB_SecuServiceUpdateValidityTime11makeRequestEjjtPc）：
// Acquire("DBThread.cpp",0x88d4) → CStreamGuard(stream,true)
//   → <<0x1c9(int) → <<-1(int)
//   → sig=GetInBuffer<SIG_SECURITY_CARD_SERVICE_VALIDITYTIME>()
//   → sig+0x00(uint)=accId、sig+0x04(uint)=type、sig+0x08(ushort)=flag
//   → strncpy(sig+0x0a, uid, 0x10) → put(DB_Q(2))。
void DB_SecuServiceUpdateValidityTime::makeRequest(unsigned int accId,
                                                   unsigned int type,
                                                   unsigned short flag,
                                                   char* uid)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x88d4), true);
    guard.operator*()->operator<<((int)0x1c9);
    guard.operator*()->operator<<((int)-1);
    SIG_SECURITY_CARD_SERVICE_VALIDITYTIME* sig =
        guard.operator->()->GetInBuffer<SIG_SECURITY_CARD_SERVICE_VALIDITYTIME>();
    sig->m_mId = accId;
    sig->m_type = type;
    sig->m_flag = flag;
    strncpy(sig->m_uid, uid, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}