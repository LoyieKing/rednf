void DB_UpdateAvatarHistory::makeRequest(unsigned int characNo, int avatarUid,
                                         char* ipgNo)
{
    // P0：Acquire 返回值右值内联进 ctor（ORIG %eax 直通，无 stream 栈槽）。
    // 每个 operator<< 前有独立 operator*(deEv)；put_binary 前是 operator->(ptEv)
    // ——仓内惯用显式代理链（user-defined operator* 返回指针，*guard << v 不可编译）。
    // characNo 为 uint → 选中 lsEj（0x080e2292）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xac3b), true);
    guard.operator*()->operator<<((int)0x2b0);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<(characNo);       // lsEj
    guard.operator*()->operator<<(avatarUid);      // lsEi
    guard.operator->()->put_binary(ipgNo, 0x18);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
