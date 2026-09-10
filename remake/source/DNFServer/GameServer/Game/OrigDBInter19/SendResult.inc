bool DB_GetUserRegdate::SendResult(int userIdx, int slot,
                                   SIG_GET_USER_REGDATE* sig)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa4c1), true);

    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    guard->put_binary(sig, 0xc);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
