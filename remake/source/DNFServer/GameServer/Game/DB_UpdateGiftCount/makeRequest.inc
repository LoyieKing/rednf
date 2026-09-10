void DB_UpdateGiftCount::makeRequest(unsigned int accId, int count)
{
    // [b28d-53 P259] ORIG 无 stream 具名局部：Acquire 返回值 eax 直传 guard ctor
    // 的 0x4(%esp)；具名缓存多 1 store+1 load 并使 guard 槽 -0x10→-0x14。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6841), true);
    guard.operator*()->operator<<(0xd1);              // SIG_UPDATE_GIFT_COUNT
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);
    guard.operator*()->operator<<(count);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
