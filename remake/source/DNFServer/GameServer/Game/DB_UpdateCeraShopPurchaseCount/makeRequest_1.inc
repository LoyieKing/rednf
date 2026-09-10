void DB_SavePuUserCheck::makeRequest(unsigned int accId)
{
    // P0/P183：Acquire 返回值右值内联进 CStreamGuard ctor（ORIG %eax 直通，
    // 无 stream 栈槽）。
    // ORIG 每个 operator<< 前有一次独立的 operator* 调用（deEv 返回值作 lsEi/lsEj
    // 的 this）。user-defined operator* 返回指针，`*guard << v` 不可编译，
    // 仓内统一惯用 guard.operator*()->operator<<(v) 显式代理链。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbcf4), true);
    guard.operator*()->operator<<((int)0x2e2);   // SIG opcode
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<(accId);        // uint → lsEj
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
