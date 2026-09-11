// ============================================================================
// DB_UpdateInventoryCapacity::makeRequest @ 0x08444d12
// ============================================================================
void DB_UpdateInventoryCapacity::makeRequest(ENUM_SERVER_GROUP serverGroup,
                                             int /*uid*/, unsigned int characNo,
                                             int capacity)
{
    // [b28c-32] P0/P103：ORIG 0x08444d32 Acquire 结果 %eax 直传 guard C1 的
    // 0x4(%esp)（无具名 stream 局部槽；guard 落 -0x10 而非 -0x14）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xabd0), true);
    guard.operator*()->operator<<(0x2ae);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(static_cast<int>(serverGroup));
    guard.operator*()->operator<<(characNo);   // 走 operator<<(unsigned int) 0x080e2292
    guard.operator*()->operator<<(capacity);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}