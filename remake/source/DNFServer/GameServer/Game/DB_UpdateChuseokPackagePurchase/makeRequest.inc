// ============================================================================
// DB_UpdateChuseokPackagePurchase::makeRequest @ 0x0843c8b6
// mangled _ZN31DB_UpdateChuseokPackagePurchase11makeRequestEj
// ============================================================================
void DB_UpdateChuseokPackagePurchase::makeRequest(unsigned int mId)
{
    // b28c-33：ORIG 0x0843c8b6 无 stream 具名局部（Acquire 返回值 eax 直传 guard
    // ctor 0x4(%esp)，guard 槽 -0x10）；值缓存局部多 store/load 且槽平移 -0x14。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x90bd), true);
    guard.operator*()->operator<<(0x213);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(mId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
