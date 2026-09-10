void ServiceRestrictManager::loadPolicy()
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/System/ServiceRestrictManager.cpp", 0x1c5), true);
    Taiwan::internal_stream(guard, TaiwanInternalPack::T(1), 0xffffffff);
    Taiwan::SigLoadRestrictPolicy* sig =
        guard->GetInBuffer<Taiwan::SigLoadRestrictPolicy>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0x1cc,
                              "[Taiwan, Restrict] SigLoadRestrictPolicy is null.");
    }
    else
    {
        // ORIG 0x0816e8ab：GetInBuffer 后先 sig->init() 清零再入 DB 队列。
        sig->init();
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
