void DBSelectCreateDnfEventInfo::makeRequest()
{
    CEventCreateDnf* event =
        static_cast<CEventCreateDnf*>(GlobalData::s_event_manager->GetRepeatEvent(0xa0));
    // [b28d-30] 真语义 bug 修正：ORIG 0815bab4..0815bae9 语义 = body 执行当且仅当
    // `event != 0 && IsEventing(0)==1`（Ghidra C：cVar2==1 → bVar1=false → !bVar1 进 body）。
    // 旧码 `event==0 || IsEventing(0)` 极性相反（IsEventing==0 才进 body）。修法 =
    // 早返形 `if (event==0 || !IsEventing(0)) return;`——探针 /tmp/probe30_f.cpp m1
    // 逐条复现 ORIG（je L_true / xor $1 / je L_false / test;jne 跳过 body 岛）。
    if (event == 0 || !event->IsEventing(0))
        return;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire(
            "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x1e4),
        true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(9), -1);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
