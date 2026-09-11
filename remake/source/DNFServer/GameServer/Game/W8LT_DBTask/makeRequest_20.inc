// ORIG 0x08437c50 sz 0xfe（_ZN24DB_Find_Factory_Hub_User11makeRequestEjPct；
// objdump 转写，报告目录仅有 dispatch.md）：Acquire("DBThread.cpp",0x851e)
//   → CStreamGuard(stream,true) → <<0x1a0(int) → <<uid(int)
//   → sig=GetInBuffer<SIG_FIND_FACTORY_HUB_USER>()
//   → strncpy(sig+0x00, name, 0x1d) → *(ushort*)(sig+0x1e)=flag
//   → put(DB_Q(2))。
void DB_Find_Factory_Hub_User::makeRequest(unsigned int uid, char* name,
                                           unsigned short flag)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x851e), true);
    guard.operator*()->operator<<((int)0x1a0);
    guard.operator*()->operator<<((int)uid);
    SIG_FIND_FACTORY_HUB_USER* sig =
        guard->GetInBuffer<SIG_FIND_FACTORY_HUB_USER>();
    strncpy(sig->m_characName, name, 0x1d);
    sig->m_flag = flag;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}