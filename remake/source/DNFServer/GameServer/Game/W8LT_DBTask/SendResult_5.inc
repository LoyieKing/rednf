// ORIG 0x841a1a2：Acquire("DBThread.cpp",0x4c23) → CStreamGuard(stream,true)
// → <<userIdx(int) → <<slot(int) → put_binary(sig,0x180c) → put(DISPATCH_Q) 返 1。
bool DB_SaveDungeonClear::SendResult(int userIdx, int slot,
                                     SIG_SAVE_DUNGEON_CLEAR* sig)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x4c23), true);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((int)slot);
    guard.operator->()->put_binary(sig, 0x180c);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}