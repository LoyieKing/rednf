void DB_LoadDnfHackLog::makeRequest(int uid, unsigned int accId)
{
    // ORIG 0x084224ce：Acquire 结果从未落栈槽（P0 右值内联），无 Stream* 具名局部。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5abf), true);
    guard.operator*()->operator<<(0x106);
    guard.operator*()->operator<<(uid);
    // P103：ORIG 先调 operator-> 再 GetInBuffer（Ghidra C 明确 pCVar2=operator->(local_18);
    // local_10=GetInBuffer<T>(pCVar2)）。
    SIG_LOAD_DNF_HACK_LOG* sig =
        guard.operator->()->GetInBuffer<SIG_LOAD_DNF_HACK_LOG>();
    *reinterpret_cast<unsigned int*>(sig) = accId;   // ORIG *(uint*)local_10 = param_2
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
