void DB_UpdateReturnUserFirstLogin::makeRequest(unsigned int accId, int a2, int a3)
{
    // ORIG Acquire 结果直传构造（无 stream 局部槽，P89/P134）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x92e1), true);
    guard.operator*()->operator<<(0x21f);  // SIG opcode（dbRequest 头）
    guard.operator*()->operator<<(-1);     // slot/mask 固定 -1
    guard.operator*()->operator<<(accId);  // +0x08 参数1（unsigned int）
    guard.operator*()->operator<<(a3);     // +0x10 参数3 先写
    guard.operator*()->operator<<(a2);     // +0x0c 参数2 后写

    GlobalData::s_msgq_mgr->put(
        static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);   // MsgQueueMgr::DB_Q
