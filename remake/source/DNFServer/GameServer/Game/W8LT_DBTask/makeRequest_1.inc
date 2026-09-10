void DB_BlackipPrivateSecurityValuation::makeRequest(unsigned int accId)
{
    // ORIG 无 stream 局部（Acquire 返回值直传 guard ctor），链式保持帧形态。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7fa1), true);
    // ORIG 每条 << 前均先 call CStreamGuard::operator*（0x080c8c46），逐指令对齐。
    guard.operator*()->operator<<((int)0x164);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)accId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
