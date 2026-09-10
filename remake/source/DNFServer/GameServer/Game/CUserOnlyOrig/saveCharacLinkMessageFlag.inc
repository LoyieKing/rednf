void CUser::saveCharacLinkMessageFlag(bool flag)
{
    setCharacLinkMessageFlag(flag);
    // ORIG 0x086528f2：`call Acquire; movl $1,0x8(%esp); mov %eax,0.4→0x4(%esp);
    // lea -0x10(%ebp)` Acquire 结果免槽直落（技巧37）；具名 s 槽致 guard 位移。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1c66), true);
    guard.operator*()->operator<<((int)0x1e4);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<(get_acc_id());
    guard.operator*()->operator<<(flag);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
