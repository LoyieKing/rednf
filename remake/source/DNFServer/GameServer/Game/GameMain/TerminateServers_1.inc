void TerminateServers()
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire((const char*)"App.cpp", 0x2e2a), true);
    **guard << 0x3e;
    **guard << 0;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
