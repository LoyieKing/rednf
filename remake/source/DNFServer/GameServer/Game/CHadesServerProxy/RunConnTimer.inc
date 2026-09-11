void CHadesServerProxy::RunConnTimer()
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DF_HadesServerProxy.cpp", 0x119),
                       true);
    (**guard) << 0x23a;
    (**guard) << -1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}