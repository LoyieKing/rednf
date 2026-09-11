// ============================================================================
// DB_ReqItemUpgradeLog::makeRequest @ 0x08425ece
// ============================================================================
void DB_ReqItemUpgradeLog::makeRequest(unsigned int a, unsigned int b,
                                       unsigned int c, const char* str,
                                       unsigned int d, bool e)
{
    // [b28c-L29] ORIG 0x08425ece：Acquire 返回值直接作 CStreamGuard ctor 实参
    // （无具名 stream 局部槽，P0）——guard 槽 -0x10 与 ORIG 一致。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x63f7),
                       true);
    guard.operator*()->operator<<(0xc0);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(a);
    guard.operator*()->operator<<(b);
    guard.operator*()->operator<<(c);
    guard.operator*()->operator<<(d);
    guard.operator*()->operator<<(e);
    // ORIG：param_4 为 char*，直接写入（const_cast 适配本声明）。
    guard.operator*()->operator<<(const_cast<char*>(str));
    // ORIG 队列 idx=4（LOGDB_Q）；MsgQueueMgr::put(QUEUE_IDX, CStreamGuard&)。
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
}