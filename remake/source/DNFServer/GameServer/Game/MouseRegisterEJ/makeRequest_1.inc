void DB_CheckSecuReward::makeRequest(int param1, unsigned int uid, int param3)
{
    // ORIG T 0x08444408：Acquire(__FILE__,0xab34) → <<0x2ab <<param1
    // → GetInBuffer 写 uid@+0 param3@+4 → put DB_Q。
    // [P1T4 #284-303] ORIG 0x0844441d `movl $0x8c36714,0x4(%esp)` = "DBThread.cpp"
    // （与上一件同址）⇒ 字面串形式，理由同上。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xab34), true);
    **guard << 0x2ab;
    **guard << param1;
    SIG_CHECK_USER_SECU_REWARD* sig =
        guard->GetInBuffer<SIG_CHECK_USER_SECU_REWARD>();
    reinterpret_cast<SIG_CSR_WRITEVIEW*>(sig)->m_uid = uid;
    reinterpret_cast<SIG_CSR_WRITEVIEW*>(sig)->m_param3 = param3;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}