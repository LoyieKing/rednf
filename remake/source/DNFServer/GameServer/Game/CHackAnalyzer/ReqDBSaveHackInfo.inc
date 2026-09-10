void CHackAnalyzer::ReqDBSaveHackInfo(unsigned int uid, unsigned int type,
                                      unsigned short cnt, int a, int b)
{
    // SIG_SAVE_CLIENT_HACK_LOG（0x12 字节）：uid/type/cnt/a/b。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("HackAnalyzer.cpp", 0x249), true);
    **guard << 0x9a;
    **guard << (int)-1;
    SIG_SAVE_CLIENT_HACK_LOG* sig =
        guard->GetInBuffer<SIG_SAVE_CLIENT_HACK_LOG>();
    sig->m_uid = uid;
    sig->m_type = type;
    sig->m_cnt = cnt;
    sig->m_a = a;
    sig->m_b = b;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
