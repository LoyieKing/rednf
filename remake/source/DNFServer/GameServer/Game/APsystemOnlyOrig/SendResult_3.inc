// ORIG 0x8419f0a：结果回包 —— Acquire("DBThread.cpp",0x4bca) →
// CStreamGuard(stream,true) → <<userIdx → <<slot → put_binary(sig,0x1810)
// → put(DISPATCH_Q,guard) 返 1。
bool DB_LoadDungeonClear::SendResult(int userIdx, int slot,
                                     SIG_LOAD_DUNGEON_CLEAR* sig)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x4bca), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    guard.operator->()->put_binary(sig, 0x1810);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}