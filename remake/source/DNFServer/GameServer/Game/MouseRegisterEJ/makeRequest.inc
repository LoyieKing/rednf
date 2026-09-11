void DB_CancelRestingUserRestrict::makeRequest(unsigned int uid, int param2)
{
    // ORIG T 0x0843741e：Acquire(__FILE__,0x8389) → <<0x195 <<param2
    // → GetInBuffer<SIG_RESTING_USER_RESTRICT> 写 uid@+0 → put DB_Q。
    // [P1T4 #284-303 串审计/strall] ORIG 0x08437433 `movl $0x8c36714,0x4(%esp)`
    // = "DBThread.cpp"（该函数在 ORIG 中定义于 DBThread.cpp ⇒ 宏为 __FILE__；
    // 0x0844441d 同址同串）⇒ 本处须写字面串（我方文件名不同，__FILE__ 会错）。
    // 旧 __PRETTY_FUNCTION__ 使本件 O-only["DBThread.cpp"] / N-only[prett] 各 1 条。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8389), true);
    **guard << 0x195;
    **guard << param2;
    SIG_RESTING_USER_RESTRICT* sig =
        guard->GetInBuffer<SIG_RESTING_USER_RESTRICT>();
    sig->m_uid = uid;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}