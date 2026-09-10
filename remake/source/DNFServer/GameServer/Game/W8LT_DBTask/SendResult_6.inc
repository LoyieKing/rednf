bool DB_LoadKillMonsterInfo::SendResult(int userIdx, int slot,
                                        SIG_LOAD_KILL_MONSTER_INFO* sig)
{
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0xa0c1), true);

    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    guard.operator->()->put_binary(sig, 0xafcc);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
