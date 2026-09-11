// ORIG 0x082a21f9（_Z16TerminateServersv，T，189 字节/91 条）：
//   GlobalData::s_stream_pool->Acquire("App.cpp",0x2e2a) → CStreamGuard(s,true)
//   << 0x3e << 0；s_msgq_mgr->put((QUEUE_IDX)1, guard)。
void TerminateServers()
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire((const char*)"App.cpp", 0x2e2a), true);
    **guard << 0x3e;
    **guard << 0;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
}