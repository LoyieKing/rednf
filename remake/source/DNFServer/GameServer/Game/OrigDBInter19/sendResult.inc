void DB_RequestTagCharacInfo::sendResult(int param_1,
                                         SIG_REQUEST_TAG_CHARAC_INFO* sig)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb23f), true);

    guard.operator*()->operator<<(0x2fe);
    guard.operator*()->operator<<(param_1);

    SIG_REQUEST_TAG_CHARAC_INFO* out =
        guard->GetInBuffer<SIG_REQUEST_TAG_CHARAC_INFO>();
    memcpy(out, sig, 0x414c);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}