int Inter_ReadyToTerminate::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    // ORIG +0x1c：Acquire 的返回值（%eax）直接作 CStreamGuard 实参，无具名 stream 槽
    // —— 具名局部会多 `mov %eax,-0xc`+`mov -0xc,%eax` 两行并把 guard 槽推到 -0x14。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x2055), true);
    guard.operator*()->operator<<(0x3f);
    guard.operator*()->operator<<(-1);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 0;
