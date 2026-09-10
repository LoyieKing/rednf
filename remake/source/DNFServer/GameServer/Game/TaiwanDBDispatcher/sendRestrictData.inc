void LoadServiceRestrictPolicy::sendRestrictData(Taiwan::SigLoadRestrictPolicy& data)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0xd9), true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)1, -1);
    Taiwan::SigLoadRestrictPolicy* out =
        guard->GetInBuffer<Taiwan::SigLoadRestrictPolicy>();
    if (out == 0)
        throw "stream";
    out->set(data);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}
