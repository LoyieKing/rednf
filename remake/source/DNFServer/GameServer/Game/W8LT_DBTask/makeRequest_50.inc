void DB_UpdateAuraAvatarOption::makeRequest(int uid, unsigned int characNo,
                                            int idx, int value)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xabf6), true);
    // ORIG 0x0844f26：写 0x2af、-1、characNo(无符号)、idx、value 五字段；
    // uid 参数存在（符号 4 参）但 ORIG 不写入包。
    guard.operator*()->operator<<((int)0x2af);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(characNo);
    guard.operator*()->operator<<(idx);
    guard.operator*()->operator<<(value);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
